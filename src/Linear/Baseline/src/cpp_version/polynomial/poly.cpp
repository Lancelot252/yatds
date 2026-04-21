#include <iostream>
#include <map>

namespace dsaac {

/* START: polynomial implementation */

template <typename Coeff = int>
class Polynomial {
public:
    void set(int exponent, Coeff coefficient)
    {
        if (coefficient == Coeff{}) {
            terms_.erase(exponent);
        } else {
            terms_[exponent] = coefficient;
        }
    }

    Coeff coefficient(int exponent) const
    {
        auto it = terms_.find(exponent);
        return it == terms_.end() ? Coeff{} : it->second;
    }

    Polynomial operator+(const Polynomial& rhs) const
    {
        Polynomial result = *this;
        for (const auto& [exponent, coefficient] : rhs.terms_) {
            result.set(exponent, result.coefficient(exponent) + coefficient);
        }
        return result;
    }

    Polynomial operator*(const Polynomial& rhs) const
    {
        Polynomial result;
        for (const auto& [left_exp, left_coeff] : terms_) {
            for (const auto& [right_exp, right_coeff] : rhs.terms_) {
                result.set(left_exp + right_exp,
                           result.coefficient(left_exp + right_exp) + left_coeff * right_coeff);
            }
        }
        return result;
    }

    const std::map<int, Coeff, std::greater<int>>& terms() const
    {
        return terms_;
    }

private:
    std::map<int, Coeff, std::greater<int>> terms_;
};

/* END */

} // namespace dsaac

int main()
{
    dsaac::Polynomial<int> p;
    p.set(2, 3);
    p.set(0, 4);

    dsaac::Polynomial<int> q;
    q.set(1, 5);
    q.set(0, 2);

    const auto sum = p + q;
    const auto product = p * q;

    std::cout << "sum constant term: " << sum.coefficient(0) << '\n';
    std::cout << "product x^3 term: " << product.coefficient(3) << '\n';
    return 0;
}
