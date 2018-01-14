def point
{
    x : r64;
    y : r64;
    z : r64;
}

def point0
{
    x : r64 -> 0.0;
    y : r64 -> 0.0;
    z : r64 -> 0.0;
}

test_ptr(x) : (i64 **) -> bool ****
{
   &x.
}

test_atom(x) : (i64) -> atom
{
    when x > 0 -> Positive.
    else when x = 0 -> Zero.
    Negative.
}

greater_than_ten(x, y) : (i64, i64) -> (bool, bool)
{
    (x > 10, y > 10).
}

pattern_match(x, y) : (i64, i64) -> bool
{
  let (_, a) -> greater_than_ten(y, x);
  a.
}

ge(x, y) : (i64, i64) -> bool
{
    when x >= y -> true.
    else -> false.
}

le(x, y) : (i64, i64) -> bool
{
    when x <= y -> true.
    else -> false.
}

less(x, y) : (i64, i64) -> bool
{
    !ge(x, y).
}

/* /* asdfasdf */ */
fact(x) : (i32) -> i32
{
    when x = 0 -> 1.
    else -> fact(x - 1).
}
// asdf
main(argc, argv) : (i32, i8 *) -> complex
{
    let a : complex -> -3 - 8i + fact(argc);
   
    a.
}
/*