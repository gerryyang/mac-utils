--[[
test.lua 
2012-04-02 wcdj
-- --]]

-- defines a factorial function
function fact (n)
	if n == 0 then
		return 1
	else
		return n*fact(n-1)
	end
end

function norm(x, y)
	local n2 = x^2 + y^2
	return math.sqrt(n2)
end

function twice (x)
	return 2*x
end


print("enter a number: ")
input = io.read() -- read a number
data_in = tonumber(input)

if data_in == nil then
	print("invalid number")
else
	print(fact(data_in))
end

print(tostring(10) == "10") -- true
print(10 .. "" == "10")		-- true

--[[
arg[-3] = "lua"
arg[-2] = "-e"
arg[-1] = "sin=math.sin"
arg[0]  = "script"
arg[1]	= "a"
arg[2]  = "b"
--]]

--[[
print(arg[-3])
print(arg[-2])
print(arg[-1])
print(arg[0])
print(arg[1])
print(arg[2])
print(arg[3])
--]]

print(type("Hello world"))
print(type(10.4*3))
print(type(print))
print(type(type))
print(type(true))
print(type(nil))
print(type(type(X)))

--[[
string
number
function
function
boolean
nil
string
--]]

print(type(a))
a = 10
print(type(a))
a = "a string !"
print(type(a))
a = print
print(type(a))
a(type(a))

--[[
nil
number
string
function
function
--]]

a = "one string"
-- change string parts
b = string.gsub(a, "one", "another")
print(a)
print(b)

--[[
one string
another string
--]]

print("10" + 1)
print("10 + 1")
-- error
-- print("hello" + 1)

--[[
11
10 + 1
--]]


print(10 .. 20)
-- malformed number near '10..20'
-- print(10..20)
print("I'm " .. 10 .. " years old!")

--[[
1020
I'm 10 years old!
--]]

print(4 and 5)
print(nil and 6)
print(false and 13)
print(4 or 5)
print(false or 5)

--[[
5
nil
false
4
5
--]]

print(not nil)
print(not false)
print(not 0)
print(not not nil)

--[[
true
true
false
false
--]]

print("Hello " .. "World")
print(0 .. 1)

--[[
Hello World
01
--]]

days = {"Sunday", 
		"Monday", 
		"Tuesday", 
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		}
print(days[4])

a = {x = 100, y = 200; "one", "two", "three"}
print(a.x)
print(a[0])
print(a[1])
print(a[3])
print(a[4])

b = {}
b.x = 300
b.y = 400
print(b.x)


--[[
Wednesday
100
nil
one
three
nil
300
--]]

a, b, c = 10, 2^4, "wcdj"
print(a)
print(b)
print(c)
a, b, c = 0
print(a)
print(b)
print(c)

--[[
10
16
wcdj
0
nil
nil
--]]

-- swap
x = 1
y = 3
x, y = y, x
print(x)
print(y)

--[[
3
1
--]]

x = 100
if "wcdj" ~= "gerry" then
	local x
	x = 1;
	print(x)
else
	print(x)
end
print(x)

--[[
1
100
--]]

x = 100
local x = 1
print(x)
do
	local x = 2
	print(x)
end
print(x)

--[[
1
2
1
--]]

print(8*9, 1/3)
a = math.sin(3) + math.cos(10)
print(os.date())

--[[
72      0.33333333333333
Mon Apr  2 14:54:02 2012
--]]

print "Hello World"
print [[My name is wcdj]]

--[[
Hello World
My name is wcdj
--]]

a, b = string.find("hello Lua users", "Lua")
print(a, b)

--[[
7       9
--]]

function maximum(a)
	local max_idx = 1
	local max = a[max_idx]

	for i, val in ipairs(a) do
		if val > max then
			max_idx = i
			max = val
		end
	end
	return max, max_idx
end

print(maximum({8, 10, 23, 12 ,5}))
print(maximum{8, 10, 23, 12 ,5})

--[[
23      3
23      3
--]]

function newCounter()
	local i = 0
	return function() -- anonymous function
		i = i + 1
		return i
	end
end

add = newCounter()
print(add())
print(add())

--[[
1
2
--]]

Lib_wcdj = {}
Lib_wcdj.add = function (x, y) return x + y end
Lib_wcdj.subtract = function (x, y) return x - y end

Lib_gerry = {
	multiply = function (x, y) return x * y end, 
	divide = function (x, y) return x / y end
}
print(Lib_wcdj.add(1, 2))
print(Lib_wcdj.subtract(1, 2))
print(Lib_gerry.multiply(2, 3))
print(Lib_gerry.divide(5, 2))

--[[
3
-1
6
2.5
--]]



