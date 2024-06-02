#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM < 502
	#define luaL_newlib( L, l ) ( lua_newtable( L ), luaL_register( L, NULL, l ) ) 
#endif

#define check( test ) if( !( test ) ) { goto error; }

static int setuidgid( uid_t const uid, gid_t const gid ) {
	if( setresgid( gid, gid, gid ) == -1 || setresuid( uid, uid, uid ) == -1 ) {
		return -1;
	}

	return 0;
}

// setuid.setuser( username )
static int luasetuid_setuser( lua_State * const L ) {
	char const * const username = luaL_checkstring( L, 1 );

	errno = 0;
	struct passwd const * const entry = getpwnam( username );
	check( entry != NULL );

	int rv = setuidgid( entry->pw_uid, entry->pw_gid );
	check( rv == 0 );

	lua_pushboolean( L, 1 );

	return 1;

error:
	lua_pushboolean( L, 0 );
	lua_pushstring( L, strerror( errno ) );

	return 2;
}

// setuid.chroot( path, username )
static int luasetuid_chroot( lua_State * const L ) {
	char const * const path = luaL_checkstring( L, 1 );
	char const * const username = lua_tostring( L, 2 );

	uid_t uid = -1;
	gid_t gid = -1;

	if( username != NULL ) {
		errno = 0;
		struct passwd const * const entry = getpwnam( username );
		check( entry != NULL );

		uid = entry->pw_uid;
		gid = entry->pw_gid;
	}

	int const rv_chroot = chroot( path );
	check( rv_chroot != -1 );

	int const rv_chdir = chdir( "/" );
	check( rv_chdir != -1 );

	if( username != NULL ) {
		int rv = setuidgid( uid, gid );
		check( rv == 0 );
	}

	lua_pushboolean( L, 1 );

	return 1;

error:
	lua_pushboolean( L, 0 );
	lua_pushstring( L, strerror( errno ) );

	return 2;
}

static struct luaL_Reg const luasetuid_lib[] = {
	{ "setuser", luasetuid_setuser },
	{ "chroot", luasetuid_chroot },
	{ NULL, NULL },
};

LUALIB_API int luaopen_setuid( lua_State * const L ) {
	luaL_newlib( L, luasetuid_lib );

	return 1;
}
