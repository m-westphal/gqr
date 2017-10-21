// -*- C++ -*-
#ifndef RELATION_FIXED_BITSET_TEST_H
#define RELATION_FIXED_BITSET_TEST_H

#include <utility>

#include "gqrtl/RelationFixedBitset.h"

#include "TestSuite.h"

class RelationTesterAbstract
{
    public:
        virtual void testZeroInit() = 0;
	virtual void testIntConstructor( void ) = 0;
	virtual void testEqualityInequality( void ) = 0;
	virtual void testNone( void ) = 0;
	virtual void testArrayAndSetOperators( void ) = 0;
	virtual void testOrOperator( void ) = 0;
	virtual void testAndOperator( void ) = 0;
	virtual void testZeroShifts( void ) = 0;
	virtual void testShifts( void ) = 0;
	virtual void testOrder( void ) = 0;
	virtual void testAssorted( void ) = 0;
	virtual void testCardinality( void ) = 0;
	virtual void testIncrement( void ) = 0;
	virtual void testDecrement( void ) = 0;
	virtual void testConstIterator( void ) = 0;
	virtual void testSubset( void ) = 0;
        virtual ~RelationTesterAbstract() {} ;
};


template<class R>
class RelationTester: public RelationTesterAbstract

{
    private:
	const R zero;

    public:
        RelationTester()
	{
            R::init();
	}


	~RelationTester()
	{
	    R::clean_up();
	}


        void testZeroInit( void )
        {
            TS_ASSERT(zero.none());
            for(size_t i = 0; i < R::maxSize(); i++)
            {
                TS_ASSERT_EQUALS(zero[i], false);
            }

            TS_ASSERT_EQUALS(zero, (R()));
            TS_ASSERT_EQUALS(zero, (R(0U)));
        }


	void testIntConstructor( void )
        {
	    TS_ASSERT((R(0U)).none());
	    TS_ASSERT_EQUALS((R(0U)).lowestWord(), 0U);

            size_t val1 = 0;
	    size_t val2 = 0;
	    if (sizeof(typename R::getType) * 8 <= 8) {
		val1 = 236U;
		val2 = 121U;
	    }
	    else {
		val1 = 43987U;
		val2 = 21443U;
	    }

	    TS_ASSERT_EQUALS((R(val1)).lowestWord(), val1);
            TS_ASSERT_EQUALS((R(val2)).lowestWord(), val2);
        }


	void testEqualityInequality( void )
	{
	    unsigned int val = (R::maxSize() <= 8 ? 123U : 12345U);

            TS_ASSERT((R()) == (R()));
            TS_ASSERT((R()) == zero);
            TS_ASSERT(zero == (R()));

	    TS_ASSERT_EQUALS((R(val)), (R(val)));
            TS_ASSERT((R(val)) != zero);
            TS_ASSERT(zero != (R(val)));
        }


        void testNone( void )
	{
            TS_ASSERT(zero.none());
	    TS_ASSERT((R()).none());
        }


        void testArrayAndSetOperators( void )
	{
            for(size_t i = 0; i < R::maxSize(); i++)
	    {
                R val = zero;
		val.set(i);
		TS_ASSERT(!val.none());
		TS_ASSERT_EQUALS(val[i], true);
		val.unset(i);
		TS_ASSERT_EQUALS(val[i], false);
		TS_ASSERT(val.none());
            }

            for(size_t i = 0; i < R::maxSize(); i++)
	    {
		R val = zero;
		val.set(i);
		TS_ASSERT(!val.none());
		TS_ASSERT_EQUALS(val[i], true);

		for(size_t j = 0; j < R::maxSize(); j++)
		{
                    const R cmp = val;
                    if(i != j)
                        TS_ASSERT_EQUALS(val[j], false);

		    val.set(j);
                    TS_ASSERT_EQUALS(val[j], true);
                    TS_ASSERT_EQUALS(val[i], true);

		    if(i != j)
		    {
                        val.unset(j);
			TS_ASSERT_EQUALS(val[j], false);
		    }

                    TS_ASSERT(val == cmp);
                }
            }
        }


        void testOrOperator( void )
	{
            unsigned int val = R::maxSize() - 1;

	    const R one = R(1U);
	    TS_ASSERT_EQUALS(one, zero | one);
	    TS_ASSERT_EQUALS(one, one | zero);

	    R a;
	    a.set(val);

	    TS_ASSERT_EQUALS(one | a, a | one);

	    R b = a | one;

	    TS_ASSERT_EQUALS(b[val], true);
	    TS_ASSERT_EQUALS(b[0], true);

	    b.unset(0);
	    b.unset(val);
	    TS_ASSERT(b.none());
        }


	void testAndOperator( void )
	{
	    const R one = R(1U);
	    const R two = R(2U);
	    const R three = R(3U);

	    TS_ASSERT_EQUALS(zero, one & two);
	    TS_ASSERT_EQUALS(zero, two & one);
	    TS_ASSERT_EQUALS(one, one & three);
	    TS_ASSERT_EQUALS(one, three & one);

	    for(size_t i = 0; i < R::maxSize(); i++)
	    {
		R a;
		a.set(i);
		for(size_t j = 0; j < R::maxSize(); j++)
		{
		    R b;
		    b.set(j);
		    TS_ASSERT_EQUALS(i != j, (a & b).none());
		    if(i == j)
			TS_ASSERT((a & b)[i]);
		}
	    }
	}


	void testZeroShifts( void )
	{
	    const R one = R(1U);

	    TS_ASSERT_EQUALS((one << 0), one);
	    TS_ASSERT_EQUALS((one >> 0), one);
	}


	void testShifts( void )
	{
	    const R one = R(1U);

	    TS_ASSERT_EQUALS((one << 1).lowestWord(), 2U);
	    TS_ASSERT_EQUALS((one << 2).lowestWord(), 4U);
	    TS_ASSERT_EQUALS((one << 3).lowestWord(), 8U);

	    R two = one;
	    two <<= 1;
	    TS_ASSERT_EQUALS(two, (R(2U)));
	    two >>= 1;
	    TS_ASSERT_EQUALS(two, one);
	    two >>= 1;
	    TS_ASSERT(two.none());

	    R highest_bit = one << R::maxSize() - 1;
	    TS_ASSERT(!highest_bit.none());
	    TS_ASSERT_EQUALS(highest_bit[R::maxSize() - 1], true);
	    TS_ASSERT((highest_bit << 1).none());

	    for(size_t i = 1; i < R::maxSize(); i++)
	    {
		const R val = highest_bit >> i;
		TS_ASSERT(!val.none());

		TS_ASSERT_EQUALS(val[R::maxSize() - 1], false);
		TS_ASSERT_EQUALS(val[(R::maxSize() - 1) - i], true);

		R empty = val;
		empty.unset((R::maxSize() - 1) - i);
		TS_ASSERT(empty.none());
	    }

	    for(size_t i = 1; i < R::maxSize(); i++)
	    {
		const R val = one << i;
		TS_ASSERT(!val.none());

		TS_ASSERT_EQUALS(val[i], true);
		TS_ASSERT_EQUALS(val[0], false);

		R empty = val;
		empty.unset(i);
		TS_ASSERT(empty.none());
	    }
        }


	void testOrder( void )
	{
	    size_t val = R::maxSize();

	    for (size_t i = 0; i < val; i++)
		for (size_t j = 0; j < val; j++)
		    for (size_t k = 0; k < val; k++)
		    {
			R a = R(i) << j;
			a.set(k);

			R b = R(k) << i;
			b.set(j);

			TS_ASSERT_EQUALS( (a<b) && (a != b), !(b<a) && (a != b) );
			TS_ASSERT( !(a<b) || (a != b) );
			TS_ASSERT( !(b<a) || (a != b) );
		    }
	}


	void testAssorted( void )
	{
	    R a;

	    for (size_t i = 0; i < R::maxSize(); i++)
	    {
		a.set(i);
		a >>= i;

		TS_ASSERT_EQUALS(a, (R(1U)));
		a.unset(0);
		TS_ASSERT(a.none());
	    }

	    for (size_t i = 0; i < R::maxSize(); i++)
	    {
		a.set(0);
		a <<= i;

		TS_ASSERT_EQUALS(a[i], true);
		a.unset(i);
		TS_ASSERT_EQUALS(a[i], false);
		TS_ASSERT(a.none());
	    }
	}


	void testCardinality( void )
	{
	    R r;

	    TS_ASSERT_EQUALS(r.count(), 0);

	    for (size_t i = 0; i < R::maxSize(); i++)
	    {
		r.set(i);
		TS_ASSERT_EQUALS(r.count(), i + 1);
	    }
	}


	void testIncrement( void )
	{
	    for (size_t i = 0; i < R::maxSize(); i++)
	    {
		R r;
		r.set(i);
		R s = r;

		TS_ASSERT_EQUALS(s, r++);
		TS_ASSERT_LESS_THAN(s, r);
		TS_ASSERT_EQUALS(++s, r);
	    }
	}


	void testDecrement( void )
	{
	    for (size_t i = 0; i < R::maxSize(); i++)
	    {
		R r;
		r.set(i);
		R s = r;

		TS_ASSERT_EQUALS(s, r--);
		TS_ASSERT_LESS_THAN(r, s);
		TS_ASSERT_EQUALS(--s, r);
	    }
	}

	void testConstIterator( void ) {
		R empty;
		typename R::const_iterator it = empty.begin();
		TS_ASSERT_EQUALS(it, empty.end());

		size_t num = 0;
		for (typename R::const_iterator it2 = empty.begin(); it2 != empty.end(); ++it2, num++);
		TS_ASSERT_EQUALS(num, 0);


		{
		R r;
		r.set(4);

		it = r.begin();
		TS_ASSERT_EQUALS(*it, 4);
		++it;
		TS_ASSERT_EQUALS(it, r.end());
		}


		{
		R r;
		TS_ASSERT_DIFFERS(r.begin(), empty.begin());

		r.set(1);
		r.set(3);
		r.set(7);
		TS_ASSERT_DIFFERS(r.begin(), empty.begin());
		TS_ASSERT_DIFFERS(r.end(), empty.end());

		num = 0;
		for (typename R::const_iterator it2 = r.begin(); it2 != r.end(); ++it2, num++);
		TS_ASSERT_EQUALS(num, 3);

		it = r.begin();
		TS_ASSERT_DIFFERS(it, r.end()); TS_ASSERT_EQUALS(*it, 1); ++it;
		TS_ASSERT_DIFFERS(it, r.end()); TS_ASSERT_EQUALS(*it, 3); ++it;
		TS_ASSERT_DIFFERS(it, r.end()); TS_ASSERT_EQUALS(*it, 7); ++it;
		TS_ASSERT_EQUALS(it, r.end());
		}
	}

	void testSubset( void ) {
		R a;
		R b;

		b.set(1);
		TS_ASSERT(a.isSubsetOf(b));
		TS_ASSERT(!b.isSubsetOf(a));

		a.set(1);
		TS_ASSERT(a.isSubsetOf(b));
		TS_ASSERT(b.isSubsetOf(a));

		a.set(2);
		TS_ASSERT(!a.isSubsetOf(b));
		TS_ASSERT(b.isSubsetOf(a));
	}
};




class RelationFixedBitsetTest : public CxxTest::TestSuite
{
    private:
        std::vector<RelationTesterAbstract*> testers;


    public:
       void setUp()
       {
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<size_t,   1> >());
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<uint8_t,  1> >());
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<uint16_t, 1> >());
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<uint32_t, 1> >());
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<uint64_t, 2> >());
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<size_t, 5> >());
	   testers.push_back(new RelationTester< gqrtl::RelationFixedBitset<size_t, 10> >());
       }


       void tearDown()
       {
	    for(int i = 0; i < testers.size(); ++i)
		delete testers[i];
       }


       void testAll()
       {
           for(int i = 0; i < testers.size(); ++i)
	   {
               testers[i]->testZeroInit();
               testers[i]->testIntConstructor();
               testers[i]->testEqualityInequality();
               testers[i]->testNone();
               testers[i]->testArrayAndSetOperators();
               testers[i]->testOrOperator();
               testers[i]->testAndOperator();
               testers[i]->testZeroShifts();
               testers[i]->testShifts();
               testers[i]->testOrder();
               testers[i]->testAssorted();
               testers[i]->testCardinality();
               testers[i]->testIncrement();
               testers[i]->testDecrement();
               testers[i]->testConstIterator();
               testers[i]->testSubset();
	   }
       }
};
#endif // RELATION_FIXED_BITSET_TEST_H
