# Cyver programming language
- a staticly type compiled programming language that designed as a learning side project, the main idea of it is giving the full control of the code more then what C or C++ give, wrapped in high level features

### node:
- this language did not ready to real world application, after 3 months of developing yet it not got into the `generating binary`, it only able to analyse the code into an AST (abstracted syntax tree) and constexpr solving
      - there is no built-in types, whatever type that came with the language the user can remake it
      - licence: `MIT ©️ 05/09/2026`
      - ';' are optional
      - more free syntax
## How to use ucc in project space
- using `make` build tool, go to .prj folder run
  ``` bash
  make all # for build the project, use `ARGS="<args>"` to set arguments
  ```
  ``` bash
  make clean # to clean the project
  ```
  ``` bash
  make run # to run .prj/bin/cyv
  ```
  ``` bash
  make dbg # to use debuger, (gdb by default)
  ```
  ``` bash
  make sz # to print exectutable info including size
  ```
  ``` bash
  make opt # to strip the executable (by removing symbols)
  ```

## How to use the language
- you have modern programming concepts
    - vars
    ```
    let x: i32 // var declaration
    let y i32  // an other var declaration
    z :i32     // an other way
    w i32      // can only be use with normal types (pointers or references will counted as operation in this case)
    
    type may be gassed from experation
    
    let x= 1024
    y := -1
    
    let ref &u8      // reference 
    let mut ^u8      // mutable var 
    let const #u8    // constant var
    let optional ?u8 // optional
    let owned !u8    // safety stuff, can't be passed by value directly
    ptr :*u8         // a pointer
    arr :[512]u8     // an array
    lmb:fn()->u8     // lambda, `->` is optional
    
    #let ctv=34 // compile time variable
    
    #MACRO // defining a macro, !excpacting a block after defining
    ```

    - functions
    ```
    fn add(a i32, b i32) -> i32 // `->` is optional
        ret a+b
    
    sys exit 60 (ec i32) // syscall wrapper
    
    fn undef() // declaration without defining, `()` are optional when there are empty
    
    --> // this is the entry point directly from `_start` no main (no C runtime init)
        exit(69)
    ```

    - types
    ```
    typ point2d_t // declaring a type
        let x i32 // member
        y: i32
       
    /* enums are not like any other language,
        enm here means a type that is compatabel with literals
        syntax:
        enm[SIZE] name*/
        
    enm[4] u32
    enm[4] -i32 // `-` means its signed (mini flags property)
    
    enm[8] .f64 // `.` means float

    enm[8] :txt_t // `:` means it can hold text literals
        ptr: *ch
        len usz
    
    // normal types
    // redefining the Token struct in cyv1 project in it self
    enm TkT
        #UNK 0
        #NEW_LINE 1
        // ...
        #_EOF


    typ Tk=0 // `=0` means init every member with 0
        loc u32
        len u16
        uni
            id TkT
            n  u8
        fid u8
    
        let data uni Wrap // declaring a var `data` with typ Wrap while declating type it self
            txt: *txt_t
            i: *gmp::ibig
            f: *gmp::fbig
            c ch32
            block: *vec_t<Tk>
            let [rgid, rgsz] u16
    ```
    
    - about code blocks
    ```
    /* the block kind is not importent only in :
        type setting size     : typ[SIZE] name
        multy-var declaration :
            let [union0, union1]
            let {var0, var1}
        arrays and ranges:
            [1, 4, 9] // array
            {1, 5, 6} // structure literal
    * in other cases you can use what ever block kind you want
    */
    
    fn main [argc i32, argv: **ch8] i32 { // this work
        if argc<2; `ret 9`
        ret 8
    }
    
    fn main (argc i32, let argv **ch8) -> i32 // this also work
        if {argc<2}
            ret 9
        ret 8
    ```

    - bit-feilds
    ```
    // you can split bytes into bit in type like C++
    typ WindowFlags
        bit fullscrean,
            locked,
            resizable,
            display_on_top
        bit 2 click_rate // values from 0 to 3
        // 2 other bits left
    ```
