#pragma once

	//TODO: Ryan, please delete this comment after reading
	// You're welcome to rename these, or assign new values
	// But this is just a way to enforce a source of truth
	//	 and enforce the Command type 
	enum Command
	{
		Login = 1, GetUsers,
		DiscussionPost,
		GetUser, BanUser,
	};

	// This is throwing errors for the build.
	// Going to turn these into Macros which should work
	static constexpr size_t usernameSize = 30;
	static constexpr size_t passwordSize = 60;


