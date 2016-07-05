#include "stdinc.h"
#include "parser.hpp"
#include "error.hpp"
#include "symtable.hpp"
#include "commands.hpp"
#include "compiler.hpp"
#include "disassembler.hpp"
#include "decompiler.hpp"
#include "codegen.hpp"
#include "program.hpp"
#include "system.hpp"
#include "cpp/argv.hpp"

int compile(fs::path input, fs::path output, ProgramContext& program, const Commands& commands);
int decompile(fs::path input, fs::path output, ProgramContext& program, const Commands& commands);

const char* help_message =
R"(Usage: gta3sc [compile|decompile] file --config=<name> [options]
Options:
  --help                   Display this information
  -o <file>                Place the output into <file>
  --config=<name>          Which compilation configurations to use (gta3,gtavc,
                           gtasa). This effectively reads the data files at
                           '/config/<name>/' and sets some appropriate flags.
  --datadir=<path>         Path to where IDE and DAT files are in.
                           The compiler will still try to behave properly
                           without this, but this is still recommended.
  -pedantic                Forbid the usage of extensions not in R* compiler.
  -f[no-]half-float        Whether codegen uses GTA III half-float format.
  -f[no-]text-label-prefix Whether codegen uses GTA SA text label data type.

Decompiler Options:
  --graphviz-control-flow[=NAME] Outputs a DOT file of the control-flow of
                                 the script NAME (must be uppercase). If no
                                 name is specified, does this for all scripts.
                                 Can be used with the other --graphiz opts.
  --graphviz-call-graph[=NAME]   Ditto, but outputs a call graph.
  --graphviz-spawn-graph[=NAME]  Ditto, but creates a script creation graph.
)";

enum class Action
{
    None,
    Compile,
    Decompile,
};

int main(int argc, char** argv)
{
    // Due to main() not having a ProgramContext yet, error reporting must be done using fprintf(stderr, ...).

    // using guid instead of empty string because we want a unique char pointer.
    const char* empty_optional = "aae3b594-9843-4eeb-91a6-74158717417e";

    Action action = Action::None;
    Options options;
    fs::path input, output;
    std::string config_name;
    optional<Commands> commands; // can't construct Commands with no arguments

    fs::path datadir;
    const char* levelfile = nullptr;

    ++argv;

    if(*argv && **argv != '-')
    {
        if(!strcmp(*argv, "compile"))
        {
            ++argv;
            action = Action::Compile;
        }

        if(!strcmp(*argv, "decompile"))
        {
            ++argv;
            action = Action::Decompile;
        }
    }

    try
    {
        bool flag;

        while(*argv)
        {
            if(**argv != '-')
            {
                if(!input.empty())
                {
                    fprintf(stderr, "gta3sc: error: input file appears twice.\n");
                    return EXIT_FAILURE;
                }

                input = *argv;
                ++argv;
            }
            else if(optget(argv, "-h", "--help", 0))
            {
                fprintf(stdout, "%s", help_message);
                return EXIT_SUCCESS;
            }
            else if(const char* o = optget(argv, "-o", nullptr, 1))
            {
                output = o;
            }
            else if(optget(argv, nullptr, "-pedantic", 0))
            {
                options.pedantic = true;
            }
            else if(const char* name = optget(argv, nullptr, "--config", 1))
            {
                config_name = name;

                // TODO instead of hard-coding the flags, use a XML?
                if(config_name == "gta3")
                {
                    levelfile = "gta3.dat";
                    options.use_half_float = true;
                    options.has_text_label_prefix = false;
                }
                else if(config_name == "gtavc")
                {
                    levelfile = "gta_vc.dat";
                    options.use_half_float = false;
                    options.has_text_label_prefix = false;
                }
                else if(config_name == "gtasa")
                {
                    levelfile = "gta.dat";
                    options.use_half_float = false;
                    options.has_text_label_prefix = true;
                }
                else
                {
                    fprintf(stderr, "gta3sc: error: arbitrary config names not supported yet, must be 'gta3', 'gtavc' or 'gtasa'.\n");
                    return EXIT_FAILURE;
                }
            }
            else if(const char* path = optget(argv, nullptr, "--datadir", 1))
            {
                datadir = path;
            }
            else if(optflag(argv, "half-float", &flag))
            {
                options.use_half_float = flag;
            }
            else if(optflag(argv, "text-label-prefix", &flag))
            {
                options.has_text_label_prefix = flag;
            }
            else if(const char* script_name = optget(argv, nullptr, "--graphviz-control-flow", 1, empty_optional))
            {
                options.graphviz_control_flow = true;
                if(options.graphviz_only_script.empty())
                {
                    options.graphviz_only_script  = (script_name == empty_optional? "" : script_name);
                }
            }
            else if(const char* script_name = optget(argv, nullptr, "--graphviz-call-graph", 1, empty_optional))
            {
                options.graphviz_call_graph = true;
                if(options.graphviz_only_script.empty())
                {
                    options.graphviz_only_script = (script_name == empty_optional? "" : script_name);
                }
            }
            else if(const char* script_name = optget(argv, nullptr, "--graphviz-spawn-graph", 1, empty_optional))
            {
                options.graphviz_spawn_graph = true;
                if(options.graphviz_only_script.empty())
                {
                    options.graphviz_only_script = (script_name == empty_optional? "" : script_name);
                }
            }
            else if(optget(argv, nullptr, "--csv-command-usage", 0))
            {
                // undocumented, may be removed at any moment
                options.csv_command_usage = true;
            }
            else
            {
                fprintf(stderr, "gta3sc: error: unregonized argument '%s'\n", *argv);
                return EXIT_FAILURE;
            }
        }
    }
    catch(const invalid_opt& e)
    {
        fprintf(stderr, "gta3sc: error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    if(input.empty())
    {
        fprintf(stderr, "gta3sc: error: no input file\n");
        return EXIT_FAILURE;
    }

    if(config_name.empty())
    {
        fprintf(stderr, "gta3sc: error: no game config specified [--config=<name>]\n");
        return EXIT_FAILURE;
    }

    if(action == Action::None)
    {
        std::string extension = input.extension().string();
        if(iequal_to()(extension, ".sc"))
            action = Action::Compile;
        else if(iequal_to()(extension, ".scm"))
            action = Action::Decompile;
        else if(iequal_to()(extension, ".scc"))
            action = Action::Decompile;
        else if(iequal_to()(extension, ".cs"))
            action = Action::Decompile;
        else if(iequal_to()(extension, ".cm"))
            action = Action::Decompile;
        else
        {
            fprintf(stderr, "gta3sc: error: could not infer action from input extension (compile/decompile)\n");
            return EXIT_FAILURE;
        }
    }

    ProgramContext program(options);

    if(!datadir.empty())
    {
        if(levelfile == nullptr)
        {
            if(fs::exists(datadir / "gta.dat"))
                levelfile = "gta.dat";
            else if(fs::exists(datadir / "gta3.dat"))
                levelfile = "gta3.dat";
            else if(fs::exists(datadir / "gta_vc.dat"))
                levelfile = "gta_vc.dat";
            else
            {
                fprintf(stderr, "gta3sc: error: could not find level file (gta*.dat) in datadir '%s'.\n",
                            datadir.generic_u8string().c_str());
                return EXIT_FAILURE;
            }
        }

        try
        {
            program.load_dat(datadir / "default.dat", true);
            program.load_dat(datadir / levelfile, false);
        }
        catch(const ConfigError& e)
        {
            fprintf(stderr, "gta3sc: error: %s\n", e.what());
            return EXIT_FAILURE;
        }
    }

    try
    {
        std::vector<fs::path> config_files;
        config_files.reserve(3);

        // order matters
        config_files.emplace_back(config_name + "/constants.xml");
        if(datadir.empty()) config_files.emplace_back(config_name + "/default.xml");
        config_files.emplace_back(config_name + "/commands.xml");

        commands = Commands::from_xml(config_files);

        if(!datadir.empty())
        {
            (*commands).add_default_models(program);
        }
    }
    catch(const ConfigError& e)
    {
        fprintf(stderr, "gta3sc: error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    fs::path conf_path = config_path();
    fprintf(stderr, "gta3sc: using '%s' as configuration path.\n", conf_path.generic_u8string().c_str());

    switch(action)
    {
        case Action::Compile:
            return compile(input, output, program, *commands);
        case Action::Decompile:
            return decompile(input, output, program, *commands);
        default:
            Unreachable();
    }
}

int compile(fs::path input, fs::path output, ProgramContext& program, const Commands& commands)
{
    if(output.empty())
    {
        // TODO .cs .scc
        output = input;
        output.replace_extension(".scm");
    }

    try {
        std::vector<shared_ptr<Script>> scripts;

        //const char* input = "intro.sc";
        //const char* input = "test.sc";
        //const char* input = "gta3_src/main.sc";

        auto main = Script::create(input, ScriptType::Main);
        auto symbols = SymTable::from_script(*main, commands, program);
        symbols.apply_offset_to_vars(2);

        scripts.emplace_back(main);

        auto subdir = main->scan_subdir();

        auto ext_scripts = read_and_scan_symbols(subdir, symbols.extfiles.begin(), symbols.extfiles.end(), ScriptType::MainExtension, commands, program);
        auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript, commands, program);
        auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission, commands, program);

        // TODO handle lex/parser errors

        for(auto& x : ext_scripts)
        {
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
        }

        for(auto& x : sub_scripts)
        {
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
        }

        for(size_t i = 0; i < mission_scripts.size(); ++i)
        {
            auto& x = mission_scripts[i];
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
            scripts.back()->mission_id = i;
        }

        symbols.check_command_count(program);

        symbols.build_script_table(scripts);

        for(auto& script : scripts)
        {
            script->annotate_tree(symbols, commands, program);
        }

        // not thread-safe
        std::vector<std::string> models = Script::compute_unknown_models(scripts);

        // CompilerContext wants an annotated ASTs, if we have any error, it's possible that
        // the AST is not correctly annotated.
        if(program.has_error())
            throw HaltJobException();

        std::vector<CodeGenerator> gens;
        gens.reserve(scripts.size());
        for(auto& script : scripts)
        {
            CompilerContext cc(script, symbols, commands, program);
            cc.compile();
            gens.emplace_back(std::move(cc), program);
        }

        // Codegen expects a successful compilation.
        if(program.has_error())
            throw HaltJobException();

        size_t global_vars_size = 0;
        if(auto highest_var = symbols.highest_global_var())
        {
            global_vars_size = (*highest_var)->end_offset();
        }

        CompiledScmHeader header(CompiledScmHeader::Version::Liberty, symbols.size_global_vars(), models, scripts);

        // not thread-safe
        Expects(gens.size() == scripts.size());
        for(size_t i = 0; i < gens.size(); ++i) // zip
        {
            scripts[i]->size = gens[i].compute_labels();                    // <- maybe???! this is actually thread safe
        }                                                                   //
        Script::compute_script_offsets(scripts, header.compiled_size());    // <- but this isn't


        for(auto& gen : gens)
            gen.generate();

        if(FILE* f = u8fopen(output, "wb"))
        {
            auto guard = make_scope_guard([&] {
                fclose(f);
            });

            if(true)
            {
                CodeGeneratorData hgen(header, program);
                hgen.generate();
                write_file(f, hgen.buffer(), hgen.buffer_size());
            }

            for(auto& gen : gens)
            {
                write_file(f, gen.buffer(), gen.buffer_size());
            }
        }
        else
        {
            program.error(nocontext, "XXX");
        }

        if(program.has_error())
            throw HaltJobException();

    } catch(const HaltJobException&) {
        // TODO put a error message of compilation failed instead of zeroing output!??!!!
        FILE* f = u8fopen(output, "wb");
        if(f) fclose(f);
        return EXIT_FAILURE;
    }

    return 0;
}

int decompile(fs::path input, fs::path output, ProgramContext& program, const Commands& commands)
{
    FILE* outstream;

    if(output.empty())
    {
        outstream = stdout;
    }
    else
    {
        outstream = u8fopen(output, "wb");
        if(!outstream)
            program.fatal_error(nocontext, "Could not open file {} for writing", output.generic_u8string());
    }

    auto guard = make_scope_guard([&] {
        if(outstream != stdout) fclose(outstream);
    });

    auto opt_bytecode = read_file_binary(input);
    if(!opt_bytecode)
        program.fatal_error(nocontext, "File {} does not exist", input.generic_u8string());

    std::vector<uint8_t>& bytecode = *opt_bytecode;

    auto opt_header = DecompiledScmHeader::from_bytecode(bytecode.data(), bytecode.size(), DecompiledScmHeader::Version::Liberty);
    if(!opt_header)
        program.fatal_error(nocontext, "XXX Corrupted SCM Header (#1)");

    DecompiledScmHeader& header = *opt_header;

    BinaryFetcher main_segment { bytecode.data(), std::min(bytecode.size(), header.main_size) };
    std::vector<BinaryFetcher> mission_segments = mission_segment_fetcher(bytecode.data(), bytecode.size(), header, program);

    Disassembler main_segment_asm(program, commands, main_segment);
    std::vector<Disassembler> mission_segments_asm;
    mission_segments_asm.reserve(header.mission_offsets.size());

    // this loop cannot be thread safely unfolded because of main_segment_asm being
    // mutated on all the units.
    for(auto& mission_bytecode : mission_segments)
    {
        mission_segments_asm.emplace_back(program, commands, mission_bytecode, main_segment_asm);
        mission_segments_asm.back().run_analyzer();
    }

    if(true)
    {
        // run main segment analyzer after the missions analyzer
        main_segment_asm.run_analyzer();
        main_segment_asm.disassembly();
    }

    for(auto& mission_asm : mission_segments_asm)
    {
        mission_asm.disassembly();
    }

    if(program.opt.graphviz_control_flow
    || program.opt.graphviz_call_graph
    || program.opt.graphviz_spawn_graph)
    {
        //
        // node names:
        //   pN = proc N
        //   bN = block N
        //
        // colors:
        //  black  = direct goto
        //  red    = branch taken (in case of GOTO_IF_FALSE/GOTO_IF_TRUE)
        //  green  = branch not taken
        //  blue   = subroutine called
        //  purple = script spawned
        //

        BlockList block_list = find_basic_blocks(commands, main_segment_asm, mission_segments_asm);
        find_edges(block_list, commands);
        find_call_edges(block_list, commands);

        dynamic_bitset proc_declared(block_list.proc_entries.size());
        dynamic_bitset proc_written(block_list.proc_entries.size());
        size_t noname_id;

        auto declare_proc = [&](const ProcEntry& entry_point, bool invisible)
        {
            std::string script_name;
            size_t proc_id = block_list.proc_id(entry_point);

            if(auto opt_name = find_script_name(commands, block_list, entry_point.block_id))
                script_name = *opt_name;
            else
                script_name = fmt::format("NONAME_{}", noname_id);

            if(!proc_declared[proc_id])
            {
                proc_declared[proc_id] = true;
                if(invisible)
                    fprintf(outstream, "p%u [shape=point, style=invis, label=\"%s\"];\n", proc_id, script_name.c_str());
                else
                    fprintf(outstream, "p%u [shape=box, label=\"%s\"];\n", proc_id, script_name.c_str());
            }
        };

        auto write_proc = [&](const ProcEntry& entry_point)
        {
            ProcId i = block_list.proc_id(entry_point);
            if(proc_written[i] == false)
            {
                proc_written[i] = true;
                fprintf(outstream, "subgraph cluster_%u {\n", unsigned(i));

                declare_proc(entry_point, true);

                depth_first(block_list, entry_point.block_id, true, [&](BlockId block_id) {

                    auto& block = block_list.block(block_id);

                    fprintf(outstream, "b%u [shape=box, label=\"", unsigned(block_id));
                    for(auto it = block.begin(block_list), end = block.end(block_list); it != end; ++it)
                    {
                        size_t newline_pos = 0;
                        std::string output = DecompilerContext::decompile(*it, commands);
                        while((newline_pos = output.find('\n', newline_pos)) != std::string::npos)
                        {
                            output.replace(newline_pos, 1, "\\n");
                        }
                        fprintf(outstream, "%s", output.c_str());
                    }
                    fprintf(outstream, "\"];\n");

                    for(BlockId pred : block.pred)
                    {
                        const char* color = "black";

                        auto& pred_block = block_list.block(pred);
                        if(pred_block.succ.size() == 2)
                        {
                            auto i = std::distance(pred_block.succ.begin(), std::find(pred_block.succ.begin(), pred_block.succ.end(), block_id));
                            color = (i == 0? "red" : "green");
                        }

                        fprintf(outstream, "b%u -> b%u [color=%s];\n", unsigned(pred), unsigned(block_id), color);
                    }

                    return true;
                });

                fprintf(outstream, "}\n");
            }
        };

        fprintf(outstream, "digraph G {\n");

        for(size_t i = 0; i < block_list.proc_entries.size(); ++i)
        {
            auto& entry_point = block_list.proc_entries[i];

            switch(entry_point.type)
            {
                case ProcType::Main:
                case ProcType::Mission:
                case ProcType::Script:
                case ProcType::Subscript:
                    break;
                case ProcType::Gosub:
                    // Don't directly look at the body of gosubs when searchig for SCRIPT_NAME.
                    if(!program.opt.graphviz_only_script.empty())
                        continue;
                    break;
                default:
                    Unreachable();
            }
        
            noname_id = i;

            if(!program.opt.graphviz_only_script.empty())
            {
                auto opt_name = find_script_name(commands, block_list, entry_point);
                if(opt_name != program.opt.graphviz_only_script)
                    continue;
            }

            if(program.opt.graphviz_control_flow)
            {
                write_proc(entry_point);
                if(program.opt.graphviz_call_graph) // additionally write the graph for called subs
                {
                    depth_first(call_graph, block_list, entry_point, true, [&](const ProcEntry& subproc) {
                        write_proc(subproc);
                        return true;
                    });
                }
            }

            if(program.opt.graphviz_call_graph)
            {
                declare_proc(entry_point, false);

                depth_first(call_graph, block_list, entry_point, true, [&](const ProcEntry& proc) {

                    for(auto& pred : proc.called_from)
                    {
                        if(!program.opt.graphviz_control_flow)
                        {
                            auto target_proc_id = block_list.proc_id(proc);
                            fprintf(outstream, "p%u -> p%u [color=%s];\n", unsigned(pred.proc_id), unsigned(target_proc_id), "blue");
                        }
                        else
                        {

                            auto source_block_id = pred.block_id;
                            auto target_block_id = proc.block_id;
                            fprintf(outstream, "b%u -> b%u [color=%s];\n", unsigned(source_block_id), unsigned(target_block_id), "blue");
                        }
                    }
                
                    return true;
                });
            }

            if(program.opt.graphviz_spawn_graph)
            {
                declare_proc(entry_point, false);

                depth_first(spawn_graph, block_list, entry_point, true, [&](const ProcEntry& proc) {

                    for(auto& pred : proc.spawned_from)
                    {
                        if(!program.opt.graphviz_control_flow)
                        {
                            auto target_proc_id = block_list.proc_id(proc);
                            fprintf(outstream, "p%u -> p%u [color=%s];\n", unsigned(pred.proc_id), unsigned(target_proc_id), "purple");
                        }
                        else
                        {
                            auto source_block_id = pred.block_id;
                            auto target_block_id = proc.block_id;
                            fprintf(outstream, "b%u -> b%u [color=%s];\n", unsigned(source_block_id), unsigned(target_block_id), "purple");
                        }
                    }
                
                    return true;
                });
            }
        }

        fprintf(outstream, "}\n");
    }
    else if(program.opt.csv_command_usage)
    {
        struct CommandUsage
        {
            std::map<uint16_t, size_t> usage;
        };
        
        std::vector<std::pair<std::string, CommandUsage>> scripts;
        size_t highest_used_command = 0;

        BlockList block_list = find_basic_blocks(commands, main_segment_asm, mission_segments_asm);
        find_edges(block_list, commands);
        find_call_edges(block_list, commands);

        for(size_t i = 0; i < block_list.proc_entries.size(); ++i)
        {
            auto& entry_point = block_list.proc_entries[i];

            switch(entry_point.type)
            {
                case ProcType::Main:
                case ProcType::Mission:
                case ProcType::Script:
                case ProcType::Subscript:
                    break;
                case ProcType::Gosub:
                    continue;
                default:
                    Unreachable();
            }
        
            std::string script_name = find_script_name(commands, block_list, entry_point).value_or(fmt::format("NONAME_{}", i));

            depth_first(call_graph, block_list, entry_point, true, [&](const ProcEntry& proc)
            {
                depth_first(block_list, proc.block_id, true, [&](BlockId block_id) 
                {
                    auto& block = block_list.block(block_id);
                    for(auto it = block.begin(block_list), end = block.end(block_list); it != end; ++it)
                    {
                        if(!is<DecompiledCommand>(it->data))
                            continue;

                        auto usager_it = std::find_if(scripts.begin(), scripts.end(), [&](const std::pair<std::string, CommandUsage>& x) {
                            return x.first == script_name;
                        });

                        if(usager_it == scripts.end())
                        {
                            usager_it = scripts.emplace(scripts.end(), std::make_pair(script_name, CommandUsage{}));
                        }

                        auto id = get<DecompiledCommand>(it->data).id & 0x7FFF;
                        usager_it->second.usage[id]++;
                        if(id > highest_used_command)
                        {
                            highest_used_command = id;
                        }
                    }
                    return true;
                });
                return true;
            });
        }

        fprintf(outstream, "%s,", "Command");
        for(auto& script_pair : scripts)
        {
            fprintf(outstream, "%s,", script_pair.first.c_str());
        }
        fprintf(outstream, "\n");

        for(size_t id = 0; id < highest_used_command; ++id)
        {
            fprintf(outstream, "0x%.4X,", unsigned(id));
            for(auto& script_pair : scripts)
            {
                fprintf(outstream, "%u,", unsigned(script_pair.second.usage[id]));
            }
            fprintf(outstream, "\n");
        }
    }
    else
    {
        {
            std::string output = DecompilerContext(commands, main_segment_asm.get_data(), 0).decompile();
            fprintf(outstream, "/***** Main Segment *****/\n%s\n", output.c_str());
        }

        for(size_t i = 0; i < mission_segments_asm.size(); ++i)
        {
            auto& mission_asm = mission_segments_asm[i];
            // analyzer already ran after emplace
            mission_asm.disassembly();
            std::string output = DecompilerContext(commands, mission_asm.get_data(), 1+i).decompile();
            fprintf(outstream, "/***** Mission Segment %d *****/\n%s\n", (int)(i), output.c_str());
        }
    }

    return 0;
}
