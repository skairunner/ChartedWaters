// required because C++'s templates are stupid.

template<typename T> 
void removeInPlace(const int& i, std::vector<T>& v)
{
	for (int it = i; it < v.size()-1; it++)
	{
		v[it] = v[it + 1];
	}
	v.pop_back();
}