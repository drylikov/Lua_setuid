A Lua module that provides `setuid` and `chroot` functionality.


Requirements
------------

lua >= 5.1


Usage
-----

`setuid.setuid( username )`: Change the current process user ID (real,
effective, saved) to that of `username`.

`setuid.chroot( path, username )`: `chroot` into `path`. If `username`
is not `nil`, then change the current process user ID to that of
`username`. 

We need `setuid.chroot` to optionally accept a username for changing the
user ID after `chroot`ing since `getpwnam` is reading password file
entries behind the scenes. We need to obtain the target user ID before
`chroot`ing, however we require root to `chroot` so we need to actually
set the user ID afterwards.


Example
-------

	local setuid = require( "setuid" )

	-- in a real program you can only do one of these
	assert( setuid.chroot( "src", "notroot" ) )
	assert( setuid.chroot( "src" ) )
	assert( setuid.setuid( "notroot" ) )
