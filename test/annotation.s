; ModuleID = 'annotation.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.8.0"

@.str = private unnamed_addr constant [4 x i8] c"sky\00", section "llvm.metadata"
@.str1 = private unnamed_addr constant [13 x i8] c"annotation.c\00", section "llvm.metadata"
@.str2 = private unnamed_addr constant [4 x i8] c"foo\00", section "llvm.metadata"
@llvm.global.annotations = appending global [1 x { i8*, i8*, i8*, i32 }] [{ i8*, i8*, i8*, i32 } { i8* bitcast (i32 ()* @main to i8*), i8* getelementptr inbounds ([4 x i8]* @.str2, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8]* @.str1, i32 0, i32 0), i32 4 }], section "llvm.metadata"

define i32 @main() nounwind uwtable ssp {
  %test = alloca i32, align 4
  %test2 = alloca i32, align 4
  %1 = bitcast i32* %test to i8*
  call void @llvm.var.annotation(i8* %1, i8* getelementptr inbounds ([4 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8]* @.str1, i32 0, i32 0), i32 6)
  ret i32 0
}

declare void @llvm.var.annotation(i8*, i8*, i8*, i32) nounwind
