/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x86 version 1.4.0
** DO NOT EDIT! The original file is in "test.dasc".
*/

#line 1 "test.dasc"
#include "../../stdinc.h"
#include "../../disassembler.hpp"

#define DASM_EXTERN(ctx, addr, idx, type) resolve_extern(ctx, addr, idx, (type) != 0)

#include <dynasm/dasm_proto.h>

static int32_t resolve_extern(Dst_DECL, unsigned char* addr, unsigned int extern_idx, bool is_rel);
static int32_t resolve_extern(Dst_DECL, unsigned char* addr, const char* extern_name, bool is_rel);

#define resolve_extern_MACRO(a, b, c, d) resolve_extern(a, b, #c, d)

#include <dynasm/dasm_x86.h>

#define Dst &codegen.dstate

#define NotImplementedYet() (throw DynarecError("NotImplementedYet"))

//| .arch x86
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 20 "test.dasc"
//| .actionlist actions
static const unsigned char actions[84] = {
  144,144,144,144,255,104,237,255,80,240,32,255,252,255,53,237,255,252,255,
  181,233,255,137,5,240,129,237,255,137,133,253,240,129,233,255,184,240,32,
  237,255,139,5,240,129,237,255,139,133,253,240,129,233,255,199,5,237,237,255,
  199,133,233,237,255,85,255,232,243,129,196,239,255,185,237,232,243,255,249,
  255,252,233,245,250,15,255
};

#line 21 "test.dasc"
//| .globals lbl_
enum {
  lbl__MAX
};
#line 22 "test.dasc"
//| .section code, imports
#define DASM_SECTION_CODE	0
#define DASM_SECTION_IMPORTS	1
#define DASM_MAXSECTION		2
#line 23 "test.dasc"
//| .externnames extern_table
static const char *const extern_table[] = {
  (const char *)0
};
#line 24 "test.dasc"

//| .macro xpushi32, varg
//| | codegen.emit_pushi32(varg);
//| .endmacro

//| .macro call_extern, target
//|| auto target_ptr_r8z9_..target = resolve_extern_MACRO(Dst, nullptr, target, false);
//|   .imports
//|       ->__imp__..target:
//|       .dword target_ptr_r8z9_..target
//|   .code
//|       call dword [->__imp__..target]
//| .endmacro

struct CodeGeneratorIA32;

//template<typename T, typename It>
//auto generate_code(const T&, CodeGeneratorIA32::IterData, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData;
//auto generate_code(const DecompiledData& data, CodeGeneratorIA32::IterData, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData;

template<typename T>
static optional<int32_t> get_imm32(const T&, CodeGeneratorIA32& codegen);
static optional<int32_t> get_imm32(const ArgVariant2&, CodeGeneratorIA32& codegen);

using DynarecError = CompilerError;

#define DynarecUnexpectedValue(value) DynarecError("Unexpected {} at {}; {} == {}", #value, __func__, #value, value)


using DWORD = uint32_t;
using WORD = uint16_t;
using BYTE = uint8_t;

typedef union
{
    DWORD	dwParam;
    int		nParam;
    float	fParam;
    void *	pParam;
    char *	szParam;
} SCRIPT_VAR;

#pragma pack(push,1)
struct CScriptThread
{
    CScriptThread	*next;					//next script in queue
    CScriptThread	*prev;					//previous script in queue
    char			threadName[8];			//name of thread, given by 03A4 opcode
    BYTE			*baseIp;				//pointer to begin of script in memory
    BYTE			*ip;					//current index pointer
    BYTE			*stack[8];				//return stack for 0050, 0051
    WORD			sp;						//current item in stack
    WORD			_f3A;					//padding
    SCRIPT_VAR		tls[34];				//thread's local variables
    BYTE			isActive;				//is current thread active
    char			condResult;				//condition result (true or false)
    char			missionCleanupFlag;		//clean mission
    char			external;				//is thread external (from script.img)
    BYTE			_fC8;					//unknown
    BYTE			_fC9;					//unknown
    BYTE			_fCA;					//unknown
    BYTE			_fCB;					//unknown
    DWORD			wakeTime;				//time, when script starts again after 0001 opcode
    WORD			logicalOp;				//00D6 parameter
    BYTE			notFlag;				//opcode & 0x8000 != 0
    BYTE			wbCheckEnabled;			//wasted_or_busted check flag
    BYTE			wastedOrBusted;			//is player wasted or busted
    BYTE			_fD5;					//unknown
    WORD			_fD6;					//unknown
    DWORD			sceneSkip;				//scene skip label ptr
    BYTE			missionFlag;			//is mission thread
    BYTE			_fDD[3];				//padding
};
#pragma pack(pop)
using CRunningScript = CScriptThread;

struct tag_CRunningScript_t {};
constexpr tag_CRunningScript_t tag_CRunningScript;


#undef Dst
#define Dst &this->dstate

struct CodeGeneratorIA32
{
private:
    const Commands& commands;
    std::vector<DecompiledData> decompiled;
    char* global_vars;

public://TODO priv
    dasm_State *dstate;
    unsigned int dstate_maxpc;
    std::map<int32_t, unsigned int> pcmap;
    void* dlabels[lbl__MAX+1]; //+1 to avoid [0] sized array

    using IterData     = decltype(decompiled)::iterator;
    using opgen_func_t = std::function<IterData(const DecompiledCommand&, IterData)>;

    std::map<uint16_t, opgen_func_t> generators;

    friend auto generate_code(const DecompiledData& data, CodeGeneratorIA32::IterData, CodeGeneratorIA32& codegen)->CodeGeneratorIA32::IterData;

public:
    enum class Reg : uint8_t
    {
        // Ids must match the rN pattern of x86-64.
        // See https://corsix.github.io/dynasm-doc/instructions.html
        Eax = 0,
        Ecx = 1,
        Edx = 2,
        Ebx = 3,
        Esp = 4,
        Ebp = 5,
        Esi = 6,
        Edi = 7,
        Max_,
    };

    CodeGeneratorIA32(const Commands& commands, std::vector<DecompiledData> decompiled)
        : commands(commands), decompiled(std::move(decompiled))
    {
        dstate_maxpc = 0;
        dasm_init(&dstate, DASM_MAXSECTION);
        dasm_setupglobal(&dstate, dlabels, lbl__MAX);
        dasm_setup(&dstate, actions);

        global_vars = nullptr;

        this->init_generators();

        // MUST construct in the same order as the enum is
        this->regstates.reserve(static_cast<size_t>(Reg::Max_));
        this->regstates.emplace_back(Reg::Eax, false);
        this->regstates.emplace_back(Reg::Ecx, false);
        this->regstates.emplace_back(Reg::Edx, false);
        this->regstates.emplace_back(Reg::Ebx, false);
        this->regstates.emplace_back(Reg::Esp, true);
        this->regstates.emplace_back(Reg::Ebp, true);
        this->regstates.emplace_back(Reg::Esi, false);
        this->regstates.emplace_back(Reg::Edi, false);
    }

    CodeGeneratorIA32(const CodeGeneratorIA32&) = delete;

    CodeGeneratorIA32(CodeGeneratorIA32&&) = default;

    ~CodeGeneratorIA32()
    {
        dasm_free(&dstate);
    }

    void init_generators();

    void add_generator(int16_t opcode, opgen_func_t gen)
    {
        Expects((opcode & 0x8000) == 0);
        auto inpair = generators.emplace(opcode, std::move(gen));
        Ensures(inpair.second == true); // first time inserting
    }

    IterData run_generator(const DecompiledCommand& ccmd, IterData it)
    {
        auto genit = generators.find(ccmd.id & 0x7FFF);
        if(genit != generators.end())
            return (genit->second)(ccmd, std::move(it));
        throw DynarecError("Unsupported opcode {}", ccmd.id);
    }

    unsigned int add_label(int32_t label_param)
    {
        auto it = pcmap.find(label_param);
        if(it != pcmap.end())
            return it->second;

        ++dstate_maxpc;
        dasm_growpc(&this->dstate, dstate_maxpc);
        pcmap.emplace(label_param, dstate_maxpc - 1);
        return dstate_maxpc - 1;
    }

    unsigned int add_label(const ArgVariant2& varg)
    {
        if(auto opt_imm32 = get_imm32(varg, *this))
            return this->add_label(*opt_imm32);
        throw DynarecError("Unexpected ArgVariant2 on add_label; varg.which() == {}", varg.which());
    }


    struct RegGuard;

    struct RegState
    {
    public:
        RegState(Reg reg, bool always_allocated)
            : id(static_cast<int>(reg)), storage(unk_t{}), is_dirty(false), is_allocated(always_allocated)
        {}

    protected:
        void set_dirty()
        {
            assert(this->is_allocated);
            if(is<DecompiledVar>(this->storage))
            {
                this->is_dirty = true;
            }
            else
            {
                assert(this->is_dirty == false);
                this->storage = unk_t{};
            }
        }

        void clear_storage()
        {
            assert(this->is_allocated);
            this->storage = unk_t{};
            this->is_dirty = false;
        }

        
        void set_storage(int32_t i)
        {
            assert(this->is_allocated);
            this->storage = i;
            this->is_dirty = false;
        }
        

        void set_storage(DecompiledVar var)
        {
            assert(this->is_allocated);
            this->storage = std::move(var);
            this->is_dirty = false;
        }

        
        bool is_storage(int32_t i)
        {
            assert(this->is_allocated);
            return is<int32_t>(storage) && get<int32_t>(storage) == i;
        }
        

        bool is_storage(const DecompiledVar& var)
        {
            assert(this->is_allocated);
            return is<DecompiledVar>(storage) && get<DecompiledVar>(storage) == var;
        }

        bool is_storing_something()
        {
            assert(this->is_allocated);
            return is<DecompiledVar>(storage);
        }

        bool is_storing(const DecompiledVar& var)
        {
            return is<DecompiledVar>(storage) && get<DecompiledVar>(storage) == var;
        }
        
    private:

    protected:
        friend struct CodeGeneratorIA32;
        friend struct CodeGeneratorIA32::RegGuard;

        struct unk_t {};

        // If is_dirty=true and is_allocated=false, reg still must be flushed into context.

        bool is_allocated : 1;
        bool is_dirty     : 1;

        int id; // TODO smaller type?

        int refcount = 0;

        variant<unk_t, int32_t, DecompiledVar> storage;
    };

    struct RegGuard
    {
    public:
        RegState* operator->()              { assert(has_state); return &state.get(); }
        RegState& operator*()               { assert(has_state); return state.get(); }
        const RegState* operator->() const  { assert(has_state); return &state.get(); }
        const RegState& operator*() const   { assert(has_state); return state.get(); }

        RegGuard(const RegGuard&) = delete;

        RegGuard(RegGuard&& rhs) :
            has_state(rhs.has_state), state(rhs.state), codegen(rhs.codegen)
        {
            rhs.has_state = false;
        }

        ~RegGuard()
        {
            if(this->has_state)
            {
                if(--state.get().refcount == 0)
                {
                    codegen.get().regfree_base(static_cast<Reg>(state.get().id));
                }
            }
        }

    protected:
        friend struct CodeGeneratorIA32;

        RegGuard(CodeGeneratorIA32& codegen, RegState& state) :
            has_state(true), state(state), codegen(codegen)
        {
            ++this->state.get().refcount;
        }

        void forget()
        {
            this->has_state = false;
            --this->state.get().refcount;
        }

        bool                                            has_state;
        std::reference_wrapper<RegState>                state;
        std::reference_wrapper<CodeGeneratorIA32>       codegen;
    };

    
    std::vector<RegState> regstates;

    static constexpr int purposes_temp = 1;
    static constexpr int purposes_all  = 2;

    optional<Reg> regalloc_base(Reg reg, bool looking_for_cache = false, bool force_alloc = false)
    {
        if(looking_for_cache)
        {
            Expects(force_alloc == false);
        }

        auto i = static_cast<size_t>(reg);
        if(this->regstates[i].is_allocated == false)
        {
            if(!looking_for_cache)
            {
                if(regstates[i].is_dirty)
                {
                    if(!force_alloc)
                        return nullopt;

                    emit_flush____(regstates[i]);
                }
                Expects(regstates[i].is_dirty == false);
            }
            regstates[i].is_allocated = true;
            return static_cast<Reg>(i);
        }
        return nullopt;
    }

    optional<Reg> regalloc_base(int purposes)
    {
        // TODO algorithm routine is very crappy, improve!!!!!!!!

        size_t max_inclusive;

        auto xalloc = [this](Reg reg) -> optional<Reg>
        {
            return this->regalloc_base(reg);
        };

        auto xalloc_force = [this](Reg reg) -> optional<Reg>
        {
            return this->regalloc_base(reg, false, true);
        };

        if(purposes == purposes_temp)
        {
            // Check the temporary purposes registers first.
            if(xalloc(Reg::Eax))
                return static_cast<Reg>(Reg::Eax);
            if(xalloc(Reg::Ecx))
                return static_cast<Reg>(Reg::Ecx);
            if(xalloc(Reg::Edx))
                return static_cast<Reg>(Reg::Edx);

            max_inclusive = static_cast<size_t>(Reg::Eax);
        }
        else
        {
            Expects(purposes == purposes_all);
            max_inclusive = static_cast<size_t>(Reg::Ebx);
        }

        for(size_t k = this->regstates.size(); k > max_inclusive; --k)
        {
            auto reg = static_cast<Reg>(k - 1);
            if(xalloc(static_cast<Reg>(reg)))
                return reg;
        }

        // now force allocation
        for(size_t k = this->regstates.size(); k > max_inclusive; --k)
        {
            auto reg = static_cast<Reg>(k - 1);
            if(xalloc_force(static_cast<Reg>(reg)))
                return reg;
        }

        return nullopt;
    }

    void regfree_base(Reg reg)
    {
        auto i = static_cast<size_t>(reg);
        Expects(regstates[i].is_allocated);
        regstates[i].is_allocated = false;
    }
    

    RegGuard regalloc(int purposes)
    {
        if(auto opt_reg = regalloc_base(purposes))
        {
            auto idx = static_cast<size_t>(*opt_reg);
            Expects(regstates[idx].refcount >= 0); // not negative
            return RegGuard(*this, regstates[idx]);
        }
        else
        {
            throw DynarecError("Register allocation failed! This is a bug!!!");
        }
    }

    // find cached var
    optional<RegGuard> regcached(const DecompiledVar& var)
    {
        for(auto& state : this->regstates)
        {
            if(state.is_storing(var))
            {
                if(auto opt_reg = regalloc_base(static_cast<Reg>(state.id), true))
                {
                    Expects(static_cast<Reg>(state.id) == *opt_reg);
                    Expects(regstates[state.id].refcount >= 0); // not negative
                    return RegGuard(*this, regstates[state.id]);
                }
            }
        }
        return nullopt;
    }





    void test()
    {
        auto& codegen = *this;

        for(auto it = decompiled.begin(); it != decompiled.end(); )
        {
            it = generate_code(*it, it, codegen);
        }

        //| nop
        //| nop
        //| nop
        //| nop
        dasm_put(Dst, 0);
#line 494 "test.dasc"

        size_t code_size;
        dasm_link(&dstate, &code_size);
        std::vector<uint8_t> buf(code_size, 0);
        dasm_encode(&dstate, buf.data());
        FILE* f = fopen("C:/Users/LINK/Desktop/__asm.bin", "wb");
        void* p = buf.data();
        fwrite(buf.data(), sizeof(uint8_t), buf.size(), f);
        fputc(0xB8, f);
        fwrite(&p, sizeof(p), 1, f);
        fclose(f);
        system("C:/Users/LINK/Desktop/__asm.bat");
        system("pause>nul");
        //__debugbreak();
    }

    void emit_flush()
    {
        // flush CRunningScript context/variables
        for(auto& state : this->regstates)
        {
            emit_flush____(state);
        }
    }

    void emit_flush____(RegState& state, bool clear_storage = false)
    {
        // WOW, this is fucking hacky, maybe find a better way to do this later.
        RegGuard fake_guard(*this, state);
        auto was_allocated = fake_guard->is_allocated;
        fake_guard->is_allocated = true;
        
        this->emit_flush(fake_guard);
        if(clear_storage) fake_guard->clear_storage();

        fake_guard->is_allocated = was_allocated;
        fake_guard.forget();
    }

    void emit_flush(RegGuard& reg)
    {
        if(reg->is_dirty)
        {
            emit_movi32(get<DecompiledVar>(reg->storage), reg);
            // remove dirtness after the mov
            reg->is_dirty = false;
        }
    }

    void emit_flush_before_call()
    {
        // flush temp registers (eax, ecx, edx)
        auto& eax = this->regstates[static_cast<size_t>(Reg::Eax)];
        auto& ecx = this->regstates[static_cast<size_t>(Reg::Ecx)];
        auto& edx = this->regstates[static_cast<size_t>(Reg::Edx)];
        emit_flush____(eax, true);
        emit_flush____(ecx, true);
        emit_flush____(edx, true);
    }










    void emit_pushi32(int32_t imm32)
    {
        //| push imm32
        dasm_put(Dst, 5, imm32);
#line 566 "test.dasc"
    }

    void emit_pushi32(RegGuard& reg)
    {
        //| push Rd(reg->id)
        dasm_put(Dst, 8, (reg->id));
#line 571 "test.dasc"
    }

    void emit_pushi32(const DecompiledVar& var)
    {
        if(auto opt_reg = regcached(var))
        {
            return emit_pushi32(*opt_reg);
        }
        else
        {
            if(var.global)
            {
                //| push dword [(global_vars + var.offset)]
                dasm_put(Dst, 12, (global_vars + var.offset));
#line 584 "test.dasc"
            }
            else
            {
                auto offset = offsetof(CRunningScript, tls) + var.offset;
                //| push dword [ebp + offset]
                dasm_put(Dst, 17, offset);
#line 589 "test.dasc"
            }
        }
    }

    void emit_pushi32(const ArgVariant2& varg)
    {
        if(auto opt_imm32 = get_imm32(varg, *this))
        {
            emit_pushi32(*opt_imm32);
        }
        else if(is<DecompiledVar>(varg))
        {
            emit_pushi32(get<DecompiledVar>(varg));
        }
        else if(is<DecompiledVarArray>(varg))
        {
            // TODO
            NotImplementedYet();
        }
        else
        {
            throw DynarecUnexpectedValue(varg.which());
        }
    }







    void emit_movi32(const DecompiledVar& dst, RegGuard& reg_src)
    {
        // Don't try to use a cached register because of emit_flush calling this.
        // (Hmm, could we work around this?)

        if(dst.global)
        {
            //| mov dword[(global_vars + dst.offset)], Rd(reg_src->id)
            dasm_put(Dst, 22, (reg_src->id), (global_vars + dst.offset));
#line 628 "test.dasc"
        }
        else
        {
            auto offset = offsetof(CRunningScript, tls) + dst.offset;
            //| mov dword[ebp + offset], Rd(reg_src->id)
            dasm_put(Dst, 28, (reg_src->id), offset);
#line 633 "test.dasc"
        }

        if(!reg_src->is_storing_something())
        {
            // Don't flush!
            //  1. if reg->is_storing_something() is false, there's nothing to flush;
            //  2. this is called by a overload of emit_movi32 that be called from emit_flush(), we don't want a infinite recursion.
            Expects(reg_src->is_dirty == false);
            reg_src->set_storage(dst);
        }
    }

    void emit_movi32(RegGuard& reg_dst, int32_t imm32)
    {
        if(!reg_dst->is_storage(imm32))
        {
            emit_flush(reg_dst);
            //| mov Rd(reg_dst->id), imm32
            dasm_put(Dst, 35, (reg_dst->id), imm32);
#line 651 "test.dasc"
            reg_dst->set_storage(imm32);
        }
    }

    void emit_movi32(RegGuard& reg_dst, const DecompiledVar& src)
    {
        if(!reg_dst->is_storage(src))
        {
            emit_flush(reg_dst);
            if(src.global)
            {
                //| mov Rd(reg_dst->id), dword[(global_vars + src.offset)]
                dasm_put(Dst, 40, (reg_dst->id), (global_vars + src.offset));
#line 663 "test.dasc"
                reg_dst->set_storage(src);
            }
            else
            {
                auto offset = offsetof(CRunningScript, tls) + src.offset;
                //| mov Rd(reg_dst->id), dword[ebp + offset]
                dasm_put(Dst, 46, (reg_dst->id), offset);
#line 669 "test.dasc"
                reg_dst->set_storage(src);
            }
        }
    }

    void emit_movi32(RegGuard& reg_dst, const ArgVariant2& src)
    {
        if(auto opt_imm32 = get_imm32(src, *this))
        {
            emit_movi32(reg_dst, *opt_imm32);
        }
        else if(is<DecompiledVar>(src))
        {
            emit_movi32(reg_dst, get<DecompiledVar>(src));
        }
        else if(is<DecompiledVarArray>(src))
        {
            // TODO
            NotImplementedYet();
        }
        else
        {
            throw DynarecUnexpectedValue(src.which());
        }
    }

    void emit_movi32(const ArgVariant2& dst, const ArgVariant2& src)
    {
        if(is<DecompiledVar>(dst))
        {
            emit_movi32(get<DecompiledVar>(dst), src);
        }
        else if(is<DecompiledVarArray>(dst))
        {
            // TODO
            NotImplementedYet();
        }
        else
        {
            throw DynarecUnexpectedValue(dst.which());
        }
    }

    void emit_movi32(const DecompiledVar& var_dst, int32_t imm32)
    {
        if(auto opt_reg_dst = regcached(var_dst))
        {
            auto& reg_dst = *opt_reg_dst;
            //| mov Rd(reg_dst->id), imm32
            dasm_put(Dst, 35, (reg_dst->id), imm32);
#line 718 "test.dasc"
            reg_dst->set_dirty();
        }
        else
        {
            if(var_dst.global)
            {
                //| mov dword[(global_vars + var_dst.offset)], imm32
                dasm_put(Dst, 53, (global_vars + var_dst.offset), imm32);
#line 725 "test.dasc"
            }
            else
            {
                auto offset = offsetof(CRunningScript, tls) + var_dst.offset;
                //| mov dword[ebp + offset], imm32
                dasm_put(Dst, 58, offset, imm32);
#line 730 "test.dasc"
            }
        }
    }


    void emit_movi32(const DecompiledVar& var_dst, const ArgVariant2& src)
    {
        if(auto opt_imm32 = get_imm32(src, *this))
        {
            emit_movi32(var_dst, *opt_imm32);
        }
        else
        {
            auto rx = this->regalloc(purposes_temp);
            emit_movi32(rx, src);
            emit_movi32(var_dst, rx);
        }
    }










    void emit_push(tag_CRunningScript_t)
    {
        //| push ebp
        dasm_put(Dst, 63);
#line 761 "test.dasc"
    }

    void emit_push(const ArgVariant2& varg)
    {
        return emit_pushi32(varg);
    }

    template<typename... Args>
    void emit_ccall(const char* extern_name, Args&&... args)
    {
        auto& codegen = *this;
        auto target_ptr = resolve_extern(&this->dstate, nullptr, extern_name, false);
        this->emit_rpushes(std::forward<Args>(args)...);
        this->emit_flush_before_call();
        //| call &target_ptr
        //| add esp, (sizeof...(args) * 4)
        dasm_put(Dst, 65, (ptrdiff_t)(target_ptr), (sizeof...(args) * 4));
#line 777 "test.dasc"
    }

    template<typename... Args>
    void emit_thiscall(const char* extern_name, void* this_ptr, Args&&... args)
    {
        auto& codegen = *this;
        auto target_ptr = resolve_extern(&this->dstate, nullptr, extern_name, false);
        this->emit_rpushes(std::forward<Args>(args)...);
        this->emit_flush_before_call();
        //| mov ecx, this_ptr     // TODO abstract the mov
        //| call &target_ptr
        dasm_put(Dst, 71, this_ptr, (ptrdiff_t)(target_ptr));
#line 788 "test.dasc"
        // callee cleanup
    }

    template<typename... Args>
    void emit_stdcall(const char* extern_name, Args&&... args)
    {
        auto& codegen = *this;
        auto target_ptr = resolve_extern(&this->dstate, nullptr, extern_name, false);
        this->emit_rpushes(std::forward<Args>(args)...);
        this->emit_flush_before_call();
        //| call &target_ptr
        dasm_put(Dst, 73, (ptrdiff_t)(target_ptr));
#line 799 "test.dasc"
        // callee cleanup
    }

private:
    template<typename Arg, typename... Rest>
    void emit_rpushes(Arg&& arg, Rest&&... rest)
    {
        this->emit_rpushes(std::forward<Rest>(rest)...);
        this->emit_push(std::forward<Arg>(arg));
    }

    void emit_rpushes()
    {
    }
};

#undef Dst
#define Dst &codegen.dstate


int32_t resolve_extern(dasm_State **dstate, unsigned char* addr, unsigned int eidx, bool is_rel)
{
    return resolve_extern(dstate, addr, extern_table[eidx], is_rel);
}

int32_t resolve_extern(dasm_State **dstate, unsigned char* addr, const char* extern_name, bool is_rel)
{
    if(!strcmp("DYNAREC_RTL_Wait", extern_name))
    {
        auto extern_ptr = (unsigned char*)0x7000;
        return is_rel? int32_t(extern_ptr - (addr + 4)) : int32_t(extern_ptr);
    }
    Unreachable();
}

auto generate_code(const DecompiledCommand& ccmd, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    return codegen.run_generator(ccmd, std::move(it));
}

auto generate_code(const DecompiledLabelDef& def, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    auto label_id = codegen.add_label(-def.offset);

    // flush context, the beggining of label should have all the context in CRunningScript
    codegen.emit_flush();
    //| =>(label_id):
    dasm_put(Dst, 76, (label_id));
#line 846 "test.dasc"

    return ++it;
}

auto generate_code(const DecompiledHex& hex, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    // TODO
    return ++it;
}

auto generate_code(const DecompiledData&, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    return visit_one(it->data, [&](const auto& data) { return ::generate_code(data, std::move(it), codegen); });
}






optional<int32_t> get_imm32(const EOAL&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const DecompiledVar&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const DecompiledVarArray&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const DecompiledString&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const int8_t& i8, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i8);
}

optional<int32_t> get_imm32(const int16_t& i16, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i16);
}

optional<int32_t> get_imm32(const int32_t& i32, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i32);
}

optional<int32_t> get_imm32(const float& flt, CodeGeneratorIA32& codegen)
{
    // TODO floating point format static assert
    return reinterpret_cast<const int32_t&>(flt);
}

optional<int32_t> get_imm32(const ArgVariant2& varg, CodeGeneratorIA32& codegen)
{
    return visit_one(varg, [&](const auto& arg) { return ::get_imm32(arg, codegen); });
}





void CodeGeneratorIA32::init_generators()
{
    auto& codegen = *this;
    
    // NOP
    this->add_generator(0x0000, [&](const DecompiledCommand& ccmd, IterData it)
    {
        // TODO, this op should only appear on the top of a script!?!?
        return ++it;
    });

    // WAIT
    this->add_generator(0x0001, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        codegen.emit_ccall("DYNAREC_RTL_Wait", tag_CRunningScript, ccmd.args[0]);
        return ++it;
    });

    // GOTO
    this->add_generator(0x0002, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        auto label_id = codegen.add_label(ccmd.args[0]);

        // flush context before jumping into label
        codegen.emit_flush();
        //| jmp =>(label_id)
        //| .align 16 // nice place to put Intel's recommended alignment
        dasm_put(Dst, 78, (label_id));
#line 945 "test.dasc"

        return ++it;
    });

    // SET
    {
        auto opgen_set = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 2);
            codegen.emit_movi32(ccmd.args[0], ccmd.args[1]);
            return ++it;
        };

        this->add_generator(0x0004, opgen_set);
        this->add_generator(0x0005, opgen_set);
        this->add_generator(0x0006, opgen_set);
        this->add_generator(0x0007, opgen_set);
        this->add_generator(0x0084, opgen_set);
        this->add_generator(0x0085, opgen_set);
        this->add_generator(0x0086, opgen_set);
        this->add_generator(0x0087, opgen_set);
        this->add_generator(0x0088, opgen_set);
        this->add_generator(0x0089, opgen_set);
        this->add_generator(0x008A, opgen_set);
        this->add_generator(0x008B, opgen_set);
        this->add_generator(0x04AE, opgen_set);
        this->add_generator(0x04AF, opgen_set);
    }



}








int test_dasc(const Commands& commands, std::vector<DecompiledData> decompiled)
{
    auto ia32 = CodeGeneratorIA32(commands, std::move(decompiled));
    ia32.test();
    return 0;
}
