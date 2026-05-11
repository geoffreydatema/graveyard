::{
    >>"Graveyard is an interpreted language that feels like Python but smells like C. Except there are no keywords because everything's an operator.";
}

TOKEN TYPES
    SEMICOLON                           ;
        usage:                          >> "hello world";

    IDENTIFIER                          <letters, numbers, and underscores>
        usage:                          some_variable = 42;

    TRUEVALUE                           $
        usage:                          x = $;

    FALSEVALUE                          %
        usage:                          x = %;

    NULLVALUE                           |
        usage:                          x = |;

    NUMBER                              <integer or floating point number>
        usage:                          1.23

    STRING                              "<any ASCII printable characters>"
        usage:                          "hello world"

    FORMATTEDSTRING                     '<any ASCII printable characters>{<variable substitution>}'
        usage:                          'the value is {value}'

    ASSIGNMENT                          =
        usage:                          x = 42;

    PLUS                                +
        contextual: ADDITION            a + b
        contextual: STRINGCONCAT        "string1" + "string2"
        contextual: ARRAYAPPEND         array + newvalue

    MINUS                               -
        contextual: SUBTRACTION         a - b
        contextual: NEGATE              -x

    ASTERISK                            * 
        contextual: MULTIPLICATION      a * b
        contextual: LENGTH              *a

    FORWARDSLASH                        /
        contextual: DIVISION            a / b
        contextual: JOINPATH            "path" / "to" / "file"

    EXPONENTIATION                      **
        usage:                          a ** b

    MODULO                              /%
        usage:                          a /% b

    PLUSASSIGNMENT                      +=
        contextual: ADDITIONASSIGNMENT  a += b;
        contextual: CONCATASSIGNMENT    "string1" += "string2";
        contextual: APPENDASSIGNMENT    array + newvalue;

    SUBTRACTIONASSIGNMENT               -=
        usage:                          a -= b

    MULTIPLICATIONASSIGNMENT            *=
        usage:                          a *= b

    DIVISIONASSIGNMENT                  /=
        usage:                          a /= b

    EXPONENTIATIONASSIGNMENT            **=
        usage:                          a **= b

    MODULOASSIGNMENT                    /%=
        usage:                          a /%= b

    INCREMENT                           ++
        usage:                          x++

    DECREMENT                           --
        usage:                          x--
        
    EQUALITY                            ==
        usage:                          a == b

    INEQUALITY                          !=
        usage:                          a != b

    LEFTANGLEBRACKET                    <
        contextual: LESSTHAN            a < b
        contextual: OPENTYPE            <SomeType> = {};

    RIGHTANGLEBRACKET                   >
        contextual: GREATERTHAN         a > b
        contextual: CLOSETYPE           <SomeType> = {};

    GREATERTHANEQUAL                    >=
        usage:                          a >= b

    LESSTHANEQUAL                       <=
        usage:                          a <= b

    NOT                                 !
        usage:                          !a

    AND                                 &&
        usage:                          a && b

    OR                                  ||
        usage:                          a || b

    XOR                                 !||
        usage:                          a !|| b

    LEFTPARENTHESES                     (
        usage:                          (a + b)

    RIGHTPARENTHESES                    )
        usage:                          (a + b)

    LEFTBRACKET                         [
        contextual: OPENARRAYLITERAL    array = [<elements>];
        contextual: OPENARRAYLOOKUP     array[0] = 42;
        contextual: OPENSLICE           string[0:10:2] //also works on arrays

    RIGHTBRACKET                        ]
        contextual: CLOSEARRAYLITERAL   array = [<elements>];
        contextual: CLOSEARRAYLOOKUP    array[0] = 42;
        contextual: CLOSESLICE          string[0:10:2] //also works on arrays

    LEFTBRACE                           {
        contextual: OPENBLOCK           function {-> $;}
        contextual: OPENHASHLITERAL     hashtable = {};

    RIGHTBRACE                          }
        contextual: CLOSEBLOCK          function {-> $;}
        contextual: CLOSEHASHLITERAL    hashtable = {};
        
    PARAMETER                           &
        contextual: PARAMETER           function &parameter {}
        contextual: ERROR               ? {} , &error {} //in a try/except/default block

    RETURN                              ->
        usage:                          -> value;

    QUESTIONMARK                        ?
        contextual: IF                  ? value == 42 {-> $;}
        contextual: TRY                 ? {}
        contextual: TERNARY             variable == 42 ? value_if_true : value_if_false
        contextual: ASSERT              ? value == 42;

    COMMA                               ,
        contextual: ELSEIF              ? value == 0 {-> $;} , value = 1 {-> %;}
        contextual: EXCEPT              ? {-> $;} , {}
        contextual: DELIMITER           [1, 2, 3]

    COLON                               :
        contextual: ELSE                ? value == 42 {-> $;} : {-> %;}
        contextual: DEFAULT             ? {-> $;} , {}
        contextual: KEYVALUEDELIMITER   {key: value}
        contextual: ARGV                : //as an expression, evaluates to {"args": [], "kwargs": {}}

    TILDE                               ~
        contextual: WHILE               ~ $ {}
        contextual: DECLARATION         ~variable;

    CARET                               ^
        contextual: CONTINUE            ^;
        contextual: KEYSOF              ^hashtable //evaluates to an array of the hashtable's keys

    BACKTICK                            `
        contextual: BREAK               `;
        contextual: VALUESOF            `hashtable //evaluates to an array of the hashtable's values

    AT                                  @
        contextual: RANGEITERATION      i @ 5 {}
        contextual: STARTSTOPITER       i @ 5, 10 {}
        contextual: STARTSTOPSTEP       i @ 9, 0, -1 {}
        contextual: ARRAYITERATION      x @ array {}

    DOUBLEQUESTION                      ??
        contextual: NULLCOALESCE        x = variable ?? defaultvalue;
        contextual: EXISTS              ?? @"path/to/file/or/folder";

    PERIOD                              .
        contextual: DECIMAL             1.23
        contextual: MEMBERACCESS        type.member

    NAMESPACE                           :: (define or resolve to a namespace specified in the following code block)
        contextual: NAMESPACEDECLARE    ::namespace {}

    REFERENCE                           #
        contextual: HASHTABLELOOKUP     hashtable#"somekey"
        contextual: NAMESPACELOOKUP     ::namespace#variable
        contextual: GLOBALLOOKUP        ::#variable
        contextual: STATICLOOKUP        ::<type>#variable
        contextual: UID                 #8 //evaluates to a random 8 character hex uid

    PRINT                               >>
        usage:                          >> value, 42, "test";

    SCAN                                <<
        usage:                          input << "please enter a value: ";

    FILEIN                              :<<
        contextual: FILEREAD            file :<< "path/to/file.ext"
        contextual: LISTDIR             :<< @"path/to/folder"

    FILEOUT                             :>>
        usage:                          data :>> "path/to/file.ext"

    RAISE                               !>>
        usage:                          !>>"error";

    CASTBOOLEAN                         >b
        usage:                          x = >b value;

    CASTINTEGER                         >i
        usage:                          x = >i value;

    CASTFLOAT                           >f
        usage:                          x = >f value;

    CASTSTRING                          >s
        usage:                          x = >s value;

    CASTARRAY                           >a
        usage:                          x = >a value;

    CASTHASHTABLE                       >h
        usage:                          x = >h value;

    TYPEOF                              @@
        usage:                          ? @@ value == "integer" {}

    TIME                                :@
        usage:                          time = :@;

    WAIT                                :~
        usage:                          :~ 1000; //wait 1000 ms

    RANDOM                              :?
        usage:                          >> :?; //evaluates to a random float between 0.0 and 1.0

    EXECUTE                             !!
        contextual: COMMAND             !! @'/path/to/executable.exe --arg "value"';
        contextual: EVAL                !! "graveyard code";

    CATCONSTANT                         ^_^
        usage:                          catconst = ^_^; //evaluates to 65458655, the number chosen by Mike, my cat who stepped on my keyboard

NON-TOKEN LEXICAL ELEMENTS
    OPENGLOBALSCOPE                     ::{
    CLOSEGLOBALSCOPE                    }
    SINGLELINECOMMENT                   //
    MULTILINECOMMENT                    /* ... */
    IMPORT                              @
    ESCAPE                              \

notes for CGraveyard implementation
    @! switch to dynamic memory allocation for tokens
    @! I need to seriously consider whether or not to allow the more functional paradigm stuff like
        - nested functions (since the graveyard way of doing this would be to nest types, not functions)
        - assigning a function to another variable (which could be in another scope...like why? there is literally no reason you absolutely need to do this)
        - returning functions (I realise this is helpful for stuff like lookup tables, however again, there are always procedural alternatives to this magic behaviour)
    @! write a breadhash function which outputs hex and use that for hashing in graveyard
    @! i may regret this, however it seems to me that if i simply whitelist every function call that we write to the gyc file (that means every AST_CALL_EXPRESSION AND every "identifier(" we see inside an EVAL) that it should be possible to audit the compiled nodes afterwards and prune out any AST_FUNCTION_DECLARATION that isn't in the whitelist, it should be possible to reliably optimise the compiled code
