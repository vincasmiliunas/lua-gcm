#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <lua.h>
#include <lauxlib.h>
#include <polarssl/gcm.h>

static int gcm_crypt(lua_State *L, unsigned int mode) {
	size_t dataLen, keyLen, ivLen;
	const unsigned char *const data = (const unsigned char *)luaL_checklstring(L, 1, &dataLen);
	const unsigned char *const key = (const unsigned char *)luaL_checklstring(L, 2, &keyLen);
	const unsigned char *const iv = (const unsigned char *)luaL_checklstring(L, 3, &ivLen);

	gcm_context ctx;
	const int err1 = gcm_init(&ctx, POLARSSL_CIPHER_ID_AES, key, keyLen*8);
	if (err1) {
		return luaL_error(L, "gcm_init failed: %d", err1);
	}

	unsigned char *const output = (unsigned char *)malloc(dataLen);
	if (!output) {
		gcm_free(&ctx);
		return luaL_error(L, "malloc failed: %d", errno);
	}

	const size_t tagLen = 16;
	unsigned char tag[tagLen];
	const int err2 = gcm_crypt_and_tag(
		&ctx,
		mode,
		dataLen,
		iv, ivLen,
		NULL, 0,
		data, output,
		tagLen, tag);
	gcm_free(&ctx);
	if (err2) {
		free(output);
		return luaL_error(L, "gcm_crypt_and_tag failed: %d", err2);
	}

	lua_pushlstring(L, (const unsigned char *)output, dataLen);
	lua_pushlstring(L, (const unsigned char *)tag, tagLen);

	free(output);
	return 2;
}

static int gcm_encrypt(lua_State *L) {
	return gcm_crypt(L, GCM_ENCRYPT);
}

static int gcm_decrypt(lua_State *L) {
	return gcm_crypt(L, GCM_DECRYPT);
}

/*
	encrypt(data, key, iv) -> (data, tag)
	decrypt(data, key, iv) -> (data, tag)
*/

LUA_API int luaopen_gcm(lua_State *L) {
	lua_newtable(L);

	lua_pushcfunction(L, gcm_encrypt);
	lua_setfield(L, -2, "encrypt");
	lua_pushcfunction(L, gcm_decrypt);
	lua_setfield(L, -2, "decrypt");

	return 1;
}
