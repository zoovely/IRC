#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <iostream>

class TEST{
    private:
        int t;
        std::string ta;
    public:
        TEST(int t, std::string ta): t(t), ta(ta){};
        const int &getT(void)const {return (t);};
        bool operator<(const TEST& rhs) const 
        {
            return t < rhs.t;
        }
};

int main(void)
{
    std::vector<TEST> tes;
    TEST a(1, "a");
    TEST b(2, "b");
    tes.push_back(a);
    tes.push_back(b);
    // std::cout << tes[-1];
    // std::set<TEST &, int> hely;
    std::map<const int, std::pair<TEST &, int> > hely;
    // std::make_pair(std::make_pair("hello", "world"), 1)
    // hely[1] = std::make_pair<TEST&, int>(a, 1);
    // hely[2] = std::make_pair<TEST&, int>(b, 1);
    // hely.insert(std::make_pair<const int,  >((1, std::make_pair<TEST&, int>(a, 1))));
    // hely.insert(std::make_pair<const int, std::pair<TEST&, int> >((1, std::make_pair<TEST&, int>(a, 1))));
    // hely.insert(std::make_pair<int, std::make_pair<TEST&, int> >(1, (a, 1));
    // hely.insert(std::make_pair<int, std::make_pair<TEST&, int> >(2, (b, 2));
    tes.erase(1 + tes.begin());

    for (std::map<const int, std::pair<TEST &, int> >::iterator _it = hely.begin(); _it != hely.end(); _it++)
    {
        std::cout << "it->first " << _it->first << "\n";
        std::cout << "it->second.first " << _it->second.first.getT() << "\n";
        std::cout << "it->second second " << _it->second.second << "\n";
        std::cout << "it->second first addr" << &_it->second.first << "\n";
    }
    // TEST c(3, "c");
    // if (hely.find(b) != hely.end())
	// 	std::cout << "yeeyeye\n";

    return (0);
}
