; ModuleID = 'a.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.8.0"

define i32 @f(i32* %p) nounwind uwtable ssp {
  %1 = alloca i32*, align 8
  %i = alloca i32*, align 8
  store i32* %p, i32** %1, align 8
  %2 = load i32** %1, align 8
  store i32* %2, i32** %i, align 8
  %3 = load i32** %i, align 8
  %4 = load i32* %3, align 4
  %5 = add nsw i32 %4, 1
  ret i32 %5
}

define i32 @main(i32 %argc, i8** %argv) nounwind uwtable ssp {
  %1 = alloca i32, align 4
  %2 = alloca i8**, align 8
  store i32 %argc, i32* %1, align 4
  store i8** %argv, i8*** %2, align 8
  ret i32 0
}
