#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
//#include <string.h>
#include <fstream>
#include <vector>


#define JAN "January"
#define FEB "February"
#define MAR "March"
#define APR "April"
#define MAY "May"
#define JUN "June"
#define JUL "July"
#define AUG "August"
#define SEP "September"
#define OCT "October"
#define NOV "November"
#define DEC "December"

#define MON "M"
// "Monday"
#define TUE "T"
// "Tuesday"
#define WED "W"
//Wednesday"
#define THU "T"
//Thursday"
#define FRI "F"
//Friday"
#define SAT "S"
//Saturday"
#define SUN "S"
//Sunday" 

using namespace std;

enum class Month
{ Jan=1, Feb, Mar, Apr, May, Jun, Jul, Aug,
Sep, Oct, Nov, Dec };

enum class Day
{ Sun=0, Mon, Tue, Wed, Thu, Fri, Sat };

// obsolete, Bill needs to inherit
/*
struct Bill
{
    double bill_amount = 0.0;
	   int d;
	   Month m;
	   int y;
};
*/

/*
class Date
includes output to cout
can add days, months, years
and takes leap years into account
*/
	
class Date
{
	public:
	class Invalid{};
	Date();
	Date(int yy, Month mm, int dd):y{yy}, m{mm}, d{dd}
	{
    if(!is_valid(yy,mm,dd)) throw Invalid{};
	}
	
	int day() const { return d; };
	Month month() const { return m; };
	int year() const { return y; };
	int days() const { return days_in_month; };
    
	void add_day();
	void add_month();
	void add_year(int n);
		
	private:
	int y{2001};
	Month m{Month::Jan};
	int d{1};
	int days_in_month{0};

	
protected:
bool is_valid(int y, Month m, int d);
    
private:    
	bool leapyear(int y);
};

const Date& default_date()
{
	static Date dd {2001, Month::Jan, 1};
	return dd;
}

Date::Date() : y{default_date().year()},
	m{default_date().month()},
	d{default_date().day()}
{
	
}

void Date::add_day()
{
	d++;
	
	if(d>days_in_month)
	{
		// d first 31>30
		d = 1;
		add_month();
	}
}

void Date::add_month()
{
	if(m == Month::Dec)
	{
		m = Month::Jan;
		//do not change day
		add_year(1);
	}
	else if(m == Month::Jan && d == 29 && !leapyear(y))
	{
	// looks like it wont be March 1st    
		// enums might be the wrong choice
		m = (Month)((int)m+1);
		d = 28;
	}
	else
	{
		m = (Month)((int)m+1);
	}
	// double check
	if(!is_valid(y,m,d)) throw Invalid{}; 
}

void Date::add_year(int n)
{
	if(m == Month::Feb && d == 29
		&& !leapyear(y+n))
	{
		m = Month::Mar;
		d = 1;
	}
	y += n;
}

/*
change / create a date -> is valid
useful
*/
bool Date::is_valid(int y, Month m, int d)
{
// Julian / Gregorian    
	if(d<0) return false;
    
	if(m<Month::Jan || Month::Dec < m)
		return false;
		
	days_in_month = 31;
	
	switch(m)
	{
	case Month::Feb:
	{
		days_in_month = (leapyear(y)) ? 29 : 28;
		break;
	}
	case Month::Apr: case Month::Jun:
	case Month::Sep: case Month::Nov:
	{
		days_in_month = 30;
		break;
	}
	}
	
	if(days_in_month<d) return false;
	
	return true;
}

bool Date::leapyear(int y)
{
	return (y%4 == 0 && y%100 != 0) || y%400 == 0;
}

//
// operators
//

ostream& operator<<(ostream& os, const Date&d)
{
	return os << '(' << d.day()
					<< ',' << int(d.month())
					<< ',' << d.year()
					<< ')';
}

bool operator==(Date d1, Date d2)
{
    return(d1.year() == d2.year()
        && d1.month() == d2.month()
            && d1.day() == d2.day());
}
// end of Date class

//
// Bill
//

class Bill : public Date
{
    public:
    Bill(double sum, int yy, Month mm, int dd) : bill_sum{sum}, b_y{yy}, b_m{mm}, b_d{dd}
    {
        saved_day = dd;
        checkLastDay();
    }
    
    int bill_day() const { return b_d; }
    Month bill_month() const { return b_m; }
    int bill_year() const { return b_y; }
    int get_saved_day() const { return saved_day; }
    double bill_me() const { return bill_sum; }
    
    void minus_year();
    void minus_month();
    void minus_day();
    void restore_month_and_add();
    
    
    private:
    // a bill can be 31 if there are 30
    int saved_day{0};
    double bill_sum{0.0};
    int b_y{0};
    Month b_m{Month::Jan};
    int b_d{0};
    
    void checkLastDay();
};

ostream& operator<<(ostream& os, const Bill&b)
{
    return os << '(' << b.bill_day()
        << "," << int(b.bill_month())
        << "," << b.bill_year()
        << ')';
}

void Bill::checkLastDay()
{
    // if the day is 31 we have three
    // tries to find 28, if not
    // we have an invalid date
    int i = 3;
    while(i-- > 0)
    {
        if(!is_valid(b_y, b_m, b_d))
        {
            cout << "Invalid date! ";
            minus_day();
        }
        else
        {
            cout << "Valid! ";
            return;
        }
    }
    // couldnt find date
    throw Invalid{};
}

void Bill::restore_month_and_add()
{
    // date.day() date.month()
    // the date will be on the
    // bill date, so grab it then
    // add_month stores m which is
    // private
    // if date.add_month then we
    // miss lots of days
    // add_month Dec to Jan and
    // adds year, or checks Feb
    // for 28 or 29
    // bill needs to check all that
    // too
    
    // 1. make m protected
    // 2. copy add_month for Bill::add
    // 3. copy add_month here and
    // use saved_day
    if(b_m == Month::Dec)
	   {
		      b_m = Month::Jan;
	       b_y++;
	   }
    // now leap year needs to be
    // protected
    // maybe i dont need this else if
    // because saved day is set
    /*
    	else if(b_m == Month::Jan && b_d == 29 && !leapyear(y))
	   {
        		b_m = (Month)((int)b_m+1);
		      b_d = 28;
    	}
    */
    	else
	   {
	   	   b_m = (Month)((int)b_m+1);
	   }
    // bill day = saved
    b_d = get_saved_day();
    checkLastDay();
}

void Bill::minus_day()
{
    b_d--;
    if(b_d < 1)
    {
        minus_month();
    }
}

void Bill::minus_month()
{
    if(b_m == Month::Jan)
    {
        b_m = Month::Dec;
        b_d = 31;
        minus_year();
    }
    else
    {
        b_m = (Month)((int)b_m-1);
        b_d = 31;
    }
    
    checkLastDay();
}

void Bill::minus_year()
{
    b_y--;
    // checked in minus_month
}


//
// time point
//

tm makeTM(
const Date date)
{
	// must initialise with {} or it is garbage
	tm get_tm{};
	string sd;
	int d = date.day();
	int m = (int)date.month();
	int y = date.year();
	
	sd += to_string(y)+"-";
	sd += to_string(m)+"-";
	sd += to_string(d);
	
	istringstream ss{sd};
	ss >> get_time(&get_tm, "%y-%m-%d");
	// no check at the moment
	// works with this
	mktime(&get_tm);
	
	return get_tm;
}

int getDayOfWeek(const Date date)
{
	tm get_day = makeTM(date);
	// we dont check if the return works

	stringstream grab_number;
	grab_number << put_time(&get_day, "%w");
	string sday = grab_number.str();
	int iday = stoi(sday);
	
	return iday;
}


// some declarations put aside for now
// this wasnt what i wanted
	string printMonth(const Date& date);
	void printHeader(const Date date);
	void printWeek(const Date date);


// no, it wouldnt work
//Bill Date::checkLastDayExist(Bill day)



/*
 * ps 31, so set all the bills
 * minus_day was for Sundays
 * date.add_day, then when a bill
 * comes out, add a month, but grab
 * saved_day
 * bills are unique, they move
 * around
 * date.add_month plus restore
 * saved_day
 */

/*
 * bill does not need add_day
 * a bill is set (in stone pretty
 * much). a bill doesnt move
 * apart from sundays etc..
 * 
 * but yes the bill needs to move
 * to next month :-(
 */


//
// main
//

int main(int argc, char *argv[])
{
    	chrono::time_point<chrono::system_clock> then{chrono::system_clock::now()};	
    	time_t t = chrono::system_clock::to_time_t(then);
    	time_t t2 = 0;
    
    // the date we update sum
    std::ifstream ifs{"bank.txt"};
    double sum;
    ifs >> sum;
    
    Date date{2025, Month::Apr, 7};
    Bill plusnet {28.68, 2025, Month::Apr, 12};
        
    /*
     * if i need, i bet a second
     * try catch block would help
     */
    try
    {
        	std::cout << "Date created: " << date << std::endl;

        std::cout << "Available: " << sum << std::endl;
        
        std::cout << "Plus.net set on " << plusnet << endl;
    }
    catch(Date::Invalid)
    {
        cout << "Dates incorrect" << endl;
    }    
    catch(Bill::Invalid)
    {
        cout << "Bills incorrect" << endl;
    }
    
    //
    // start it going
    //    
    try
    {
        	int count = 10;
        	while(count>0)
        	{
        	   	chrono::time_point<chrono::system_clock> now{chrono::system_clock::now()};
    	       t2 = chrono::system_clock::to_time_t(now);
	    
        	    // every second
        	    if(t2>t)
        	   	{
           	   		  t = t2;
        	   	    plusnet.restore_month_and_add();
                cout << plusnet << endl;
                  cout << "Next plus.net is on " << plusnet << endl;
	   		
       	   	    count--;
        	   	}
        	}
    }
    catch(Date::Invalid)
    {
        cout << "Date incorrect";
    }
    catch(Bill::Invalid)
    {
        cout << "Bill incorrect";
    }
	
    cout << "Finished." << endl;
    	cout << "\n\n";
	
	return 0;
}


string printMonth(const Date& d)
{
	switch(d.month())
	{
	case Month::Jan: return JAN;
	case Month::Feb: return FEB;
	case Month::Mar: return MAR;
	case Month::Apr: return APR;
	case Month::May: return MAY;
	case Month::Jun: return JUN;
	case Month::Jul: return JUL;
	case Month::Aug: return AUG;
	case Month::Sep: return SEP;
	case Month::Oct: return OCT;
	case Month::Nov: return NOV;
	case Month::Dec: return DEC;
	default: return "mmm";
	}
}

void printHeader(
const Date date)
{
	// my console wasn't working
	int mfs = 6;
	int tt = 7;
	int ws = 9;
	mfs = 6;
	tt = 6;
	ws = 6;
	
	cout << "==========" << endl;
	tm title = makeTM(date);
	cout << asctime(&title);
	cout << "==========" << endl;
	
	cout << "|";
	cout << setfill('X');
	cout << setw(3)<< SUN << "|";
	cout << setw(3) << MON << "|";
	cout << setw(3)<< TUE << "|";
	cout << setw(3)<< WED << "|";
	cout << setw(3)<< THU << "|";
	cout << setw(3)<< FRI << "|";
	cout << setw(3)<< SAT << "|";
	
	cout << "\n\n";
}

void printWeek(const Date date)
{
	// we want the beginning of the month
	Date begin { date.year(),
		date.month(),
		1 };
	// which enum?
	int first_day = getDayOfWeek(begin);
	Day day = (Day)((int)first_day);
	
	cout << "|";
	
	// fill last month
	for(int i=0; i < first_day; ++i)
	{
		cout << setw(3) << setfill('X') << "X" << "|";
	}
	
	// number days
	for(int i=0; i<date.days();++i)
	{
		cout << setw(3)<< setfill('X')
			<< begin.day() << "|";
		
		switch(day)
		{
		case Day::Sun: day = Day::Mon; break;
		case Day::Mon: day = Day::Tue; break;
		case Day::Tue: day = Day::Wed; break;
		case Day::Wed: day = Day::Thu; break;
		case Day::Thu: day = Day::Fri; break;
		case Day::Fri: day = Day::Sat; break;
		case Day::Sat:
		{
			day = Day::Sun;
			cout << endl << endl << "|";
		}
		}
		
		begin.add_day();
		if(begin.day() > begin.days()) break;
	}
	
	Date finish {date.year(),
		date.month(),
		date.days()};
		
	int zero_sunday = 6;
	int end = zero_sunday - getDayOfWeek(finish);

	for(int i = 0; i < end; ++i)
	{
		cout << setw(3)<< setfill('X') << "X" << "|";
	}
}