//
// Created by Amy Wang on 4/22/25.
//

#ifndef GENERIC_ALGORITHMS_H
#define GENERIC_ALGORITHMS_H
template <typename InputIterator, typename Predicate>
InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
    for (;first!=last;++first) {
        if (pred(*first)) return first;
    }
    return last;
}
#endif //GENERIC_ALGORITHMS_H


