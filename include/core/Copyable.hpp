#pragma once

class Copyable
{
public:
	virtual Copyable* make_copy() = 0;
};