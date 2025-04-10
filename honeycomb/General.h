#pragma once
#include<vector>
class General {
public:
	General();
	~General();

	void Load();
	void Draw();
	void Update();
	void Unload();

private:
	double position;
};
