#pragma once


	enum Command
	{
		Login = 1, GetUsers,
		DiscussionPost,
		GetUser, BanUser,
	};


	static constexpr size_t usernameSize = 30;
	static constexpr size_t passwordSize = 60;


