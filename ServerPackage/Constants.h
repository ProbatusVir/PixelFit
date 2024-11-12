#pragma once

	//TODO: Ryan, please delete this comment after reading
	// You're welcome to rename these, or assign new values
	// But this is just a way to enforce a source of truth
	//	 and enforce the Command type 
	enum Command
	{
		Login, GetUsers,
		DiscussionPost,
		GetUser, BanUser,
	};

	static constexpr size_t usernameSize = 30;
	static constexpr size_t passwordSize = 60;


