function fact(n)
    if n == 0 then
        return 1
    else
        return n * fact(n - 1)
    end
end

function fib(k)
    if (k < 2) then
        return 1
    else
        return fib(k - 1) + fib(k - 2)
    end
end

m = read()
x = fact(m)
print(x)

x = fib(m)
print(x)