// required because C++'s templates are stupid.

template<typename T> 
void removeInPlace(const int& i, std::vector<T>& v)
{
	for (size_t it = i; it < v.size()-1; it++)
	{
		v[it] = v[it + 1];
	}
	v.pop_back();
}

template<typename K, typename T>
std::vector<K> getKeys(std::map<K, T>& mp)
{
	std::vector<K> output;
	for (auto it = mp.begin(); it != mp.end(); it++)
        {
            output.push_back(it->first);
        }
	return output;
}