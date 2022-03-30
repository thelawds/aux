--function fact(n)
--    if n == 0 then
--        return 1
--    else
--        return n * fact(n - 1)
--    end
--end
--
--function fib(k)
--    if (k < 2) then
--        return 1
--    else
--        return fib(k - 1) + fib(k - 2)
--    end
--end
--
--m = read()
--x = fact(m)
--print(x)
--
--x = fib(m)
--print(x)


-- x,y,z = "Hello", 32.3 - 7^2
x, y, z = nil, true, false
foo = 223

x = {10, 20, 30, 40, 50, structAccess = 60, [32-foo] = 70}
x[1] = 21
x.Hello = 32 - y
x.table = {10, 20, 30, 40, 50}
x.table[0] = x[2]

y, z = nil

z = 0xABC212
y = 32.43
z = "Hello, world!"
y = z

a = 21 + 93
b = 3.0 - 1.23 + a * 7
b = z
a = 0xABC - a*a*a 

a1, a2, a3 = "a1", "a2", "a3"
a1, a2, a3 = a3, a2, a1

a4 = x.table[4] - x.table[3] + x[1]