
function printmsg()
	-- ok
	--print("hello world")

	-- mock err
	print_not_exist("hello world")
end

function errorhandle(str)
	return string.upper(str)
end

