#include <vector>
#include <map>
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
    std::map<TEST, int> hely;
    TEST a(1, "a");
    hely.insert(std::make_pair(a, 1));
    TEST b(2, "b");
    hely.insert(std::make_pair(b, 2));
    TEST c(3, "c");
    if (hely.find(b) != hely.end())
		std::cout << "yeeyeye\n";

    return (0);
}
