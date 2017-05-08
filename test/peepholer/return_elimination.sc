// RUN: %gta3sc %s -O -fcleo --config=gtasa --guesser -emit-ir2 -o - | %FileCheck %s

VAR_INT x

// CHECK-L: NOP
NOP

// CHECK-NEXT-L: GOSUB @MAIN_3
GOSUB gosub
// CHECK-NEXT-L: CLEO_CALL @MAIN_1 1i8 &8 &8 &8
CLEO_CALL func 0 x x x

// CHECK-NEXT-L: TERMINATE_THIS_SCRIPT
TERMINATE_THIS_SCRIPT

{
// CHECK-NEXT-L: MAIN_1:
func:
LVAR_INT in

// CHECK-NEXT-L: ANDOR 0
// CHECK-NEXT-L: IS_INT_LVAR_EQUAL_TO_NUMBER 0@ 0i8
// CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_2
IF in = 0
    // CHECK-NEXT-L: WAIT 0i8
    WAIT 0
    // CHECK-NEXT-L: CLEO_RETURN 2i8 0@ 0@
    //               ^~~~~~~~~~~ Optimized Branch
ELSE
    // CHECK-NEXT-L: MAIN_2
    // CHECK-NEXT-L: WAIT 1i8
    WAIT 1
ENDIF
// CHECK-NEXT-L: CLEO_RETURN 2i8 0@ 0@
CLEO_RETURN 0 in in
}

// CHECK-NEXT-L: MAIN_3:
gosub:
// CHECK-NEXT-L: ANDOR 0
// CHECK-NEXT-L: IS_INT_VAR_EQUAL_TO_NUMBER &8 0i8
// CHECK-NEXT-L: GOTO_IF_FALSE @MAIN_4
IF x = 0
  // CHECK-NEXT-L: WAIT 0i8
  WAIT 0
  // CHECK-NEXT-L: RETURN
  //               ^~~~~~ Optimized Branch
ELSE
  // CHECK-NEXT-L: MAIN_4:
  // CHECK-NEXT-L: WAIT 1i8
  WAIT 1
ENDIF
// CHECK-NEXT-L: RETURN
RETURN

