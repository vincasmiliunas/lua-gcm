require 'lunit'
local GCM = require 'gcm'

module('testcase', lunit.testcase, package.seeall)

function generateBinaryString(length)
	local result = {}
	for i = 1, length do
		table.insert(result, string.char(math.random(0, 255)))
	end
	return table.concat(result)
end

math.randomseed(os.time())
local data = generateBinaryString(23)
local key = generateBinaryString(32)
local iv = generateBinaryString(16)

function testEncryptDecrypt()
	local encrypted, tag = GCM.encrypt(data, key, iv)
	lunit.assert_not_equal(data, encrypted)
	lunit.assert_equal(#data, #encrypted)
	lunit.assert_equal(16, #tag)

	local decrypted, tag2 = GCM.decrypt(encrypted, key, iv)
	lunit.assert_equal(data, decrypted)
	lunit.assert_equal(tag, tag2)
end

function testInvalidKey()
	local work = function()
		local key2 = generateBinaryString(11)
		GCM.encrypt(data, key2, iv)
	end
	local status, err = pcall(work)
	lunit.assert_false(status)
end

lunit.main()
