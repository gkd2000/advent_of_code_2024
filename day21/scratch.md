v>A
<vA>A^A
v<<A>A>^AvA^A<A>A

>vA
vA<A^>A
<vA>^Av<<A>>^A<A>vA^A

---

^<A
<Av<A>>^A
v<<A>>^Av<A<A>>^AvAA<^A>A

<^A
v<<A>^A>A
<vA<AA>>^AvA<^A>AvA^A

---

v<A
<vA<A>>^A
v<<A>A^>Av<<A>>^AvAA<^A>A

<vA
v<<A>A>^A
<vA<AA>>^AvA^AvA<^A>A

---

>^A
vA<^A>A
<vA>^Av<<A>^A>AvA^A
v<<A>A>^AvA<^A>A<vA<AA>>^AvA<^A>AvA^A<vA>^A<A>A
<vA<AA>>^AvA^AvA<^A>A<vA>^Av<<A>^A>AvA^Av<<A>A>^Av<<A>>^AAvAA<^A>A<vA>^Av<<A>^A>AvA^A<vA>^A<A>Av<<A>A>^AvA<^A>Av<<A>>^AvA^A

^>A
<Av>A^A
v<<A>>^A<vA>A^A<A>A
<vA<AA>>^AvAA<^A>Av<<A>A^>AvA^A<A>Av<<A>>^AvA^A
v<<A>A^>Av<<A>>^AAvAA<^A>A<vA^>AAv<<A>^A>AvA^A<vA<AA>>^AvA^A<Av>A^A<vA^>A<A>Av<<A>>^AvA^A<vA<AA>>^AvAA<^A>A<vA^>A<A>A

---

<^<
v<<A>^Av<A
v<A<AA>>^AvA<^A>Av<A<A>>^A

^<<
<Av<AA
v<<A>>^A<vA<A>>^AA

Pick
v>
<^
<v
^>, unless you're avoiding the empty space

If you have to avoid the empty space on the numpad, then go ^<< instead of
zig-zagging.
If you have to avoid the empty space on the arrow keys, then go v<< instead of
zig-zagging.

<vA<AA>>^AvAA<^A>Av<<A>>^AvA^A<vA>^Av<<A>^A>AAvA^Av<<A>A>^AAAvA<^A>A
<vA<AA>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A
<v<A>A<A>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A

^A<<^^A>>AvvvA
<A>Av<<AA>^AA>AvAA^A<vAAA>^A
v<<A>>^AvA^A<vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>Av<<A>A>^AAAvA<^A>A
379

7
^^^<<A
<AAAv<AA>>^A
v<<A>>^AAA<vA<A>>^AAvAA<^A>A

idea: write a recursive pullbackArrowKeys function. It takes a string with the target sequence and an integer specifying the number of robots we still need to go through/number of pullbacks we still need to do. In the base case, this number is 0, and so we return the length of the sequence required to type the target string. 