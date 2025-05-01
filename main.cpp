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

Month addMonth(const Month& month);

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
        //		m = (Month)((int)m+1);
        addMonth(m);
        d = 28;
    }
    else
    {
        addMonth(m);
        //m = (Month)((int)m+1);
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
// Date operators
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
    Bill(double sum, int yy, Month mm, int dd, bool C) : bill_sum{sum}, b_y{yy}, b_m{mm}, b_d{dd}, credit{C}
    {
        saved_day = dd;
        checkLastDay();
    }
    
    int bill_day() const { return b_d; }
    Month bill_month() const { return b_m; }
    int bill_year() const { return b_y; }
    int get_saved_day() const { return saved_day; }
    double bill_me() const { return bill_sum; }
    bool is_credit() { return credit; }
    
    void minus_year();
    void minus_month();
    void minus_day();
    void restore_month_and_add();
    void bringSundayForward();
    bool was_sunday{false};
    
    private:
    // a bill can be 31 if there are 30
    int saved_day{0};
    double bill_sum{0.0};
    bool credit{false}; // money in?
    int b_y{0};
    Month b_m{Month::Jan};
    int b_d{0};
    
    tm bill_date{};
    
    void checkLastDay();
    // some chrono stuff
    int testGetBillDayOfWeek();
    void testMakeBillTM();
};

//
// Bill operators
//

ostream& operator<<(ostream& os, const Bill&b)
{
    return os << '(' << b.bill_day()
        << "," << int(b.bill_month())
        << "," << b.bill_year()
        << ')';
}

bool operator==(Date date, Bill bill)
{
    return (date.day() == bill.bill_day()
        && date.month() == bill.bill_month()
        && date.year() == bill.bill_year());
}


// need sunday to move
//int getBillDayOfWeek(const Bill date);

void Bill::bringSundayForward()
{
    //if(getBillDayOfWeek(*this) == 0)
    if(testGetBillDayOfWeek() == 0)
    {
        minus_day();    
        was_sunday = true;
    }    
}

void Bill::checkLastDay()
{
    // if the day is 31 we have four
    // tries to find 28, if not
    // we have an invalid date
    // if the date is 1st its valid
    int i = 4;
    while(i-- > 0)
    {
        if(!is_valid(b_y, b_m, b_d))
        {
            minus_day();
        }
        else
        {
            // if sunday?
            bringSundayForward();
            return;
        }
    }
    // valid but on sunday
    // couldnt find date
    throw Invalid{};
}

//Month addMonth(const Month& month);

void Bill::restore_month_and_add()
{
    was_sunday = false;
    
    if(b_m == Month::Dec)
	   {
		      b_m = Month::Jan;
	       b_y++;
	   }
    else
	   {
	       //b_m = (Month)((int)b_m+1);
	       // safer way
	       b_m = addMonth(b_m);
	   }
    
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

Month minusMonth(const Month &month);

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
        //b_m = (Month)((int)b_m-1);
        b_m = minusMonth(b_m);
        b_d = 31;
    }
    
    checkLastDay();
}

void Bill::minus_year()
{
    b_y--;
    // checked in minus_month
}
// end of Bill


//
// time point
//

/*
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
*/

/*
 * we only want is sunday = 0
 */
/*
int getDayOfWeek(const Date date)
{
	tm get_day = makeTM(date);

	stringstream grab_number;
	grab_number << put_time(&get_day, "%w");
	string sday = grab_number.str();
	int iday = stoi(sday);
	
	return iday;
}
*/

//tm makeBillTM(
//const Bill date)

// Bill private tm?
void Bill::testMakeBillTM()
{
	// must initialise with {} or it is garbage
	//tm get_tm{};
	string sd;
    
//	int d = date.bill_day();
    
    
    int d = bill_day();
    
    
	int m = (int)bill_month();
	int y = bill_year();
	
	sd += to_string(y)+"-";
	sd += to_string(m)+"-";
	sd += to_string(d);
	
	istringstream ss{sd};
//	ss >> get_time(&get_tm, "%y-%m-%d");
    
    
    ss >> get_time(&bill_date, "%y-%m-%d");
    
    
	// no check at the moment
	// works with this
    
    
//	mktime(&get_tm);
    mktime(&bill_date);
    
    cout << " tm "<< asctime(&bill_date) << endl;
	
//	return get_tm;
}

//int getBillDayOfWeek(const Bill date)
int Bill::testGetBillDayOfWeek()
{
//	tm get_day = makeBillTM(date);
    testMakeBillTM();
    
    // use private tm?
    //tm get_day = testMakeBillTM(*this);
	// we dont check if the return works

	stringstream grab_number;
    
    
    //grab_number << put_time(&get_day, "%w");
    grab_number << put_time(&bill_date, "%w");
    
    
	string sday = grab_number.str();
	int iday = stoi(sday);
    
    cout << " iday " << iday << endl;
	
	return iday;
}


// some declarations put aside for now
// this wasnt what i wanted
	string printMonth(const Date& date);
	void printHeader(const Date date);
	void printWeek(const Date date);


vector<Bill> makeBills(const Date& date)
{
    vector<Bill> temp;
    int d = date.day();
    Month m = date.month();
    int y = date.year();
    
    int b_d = 0;
    
    b_d = 1;
    Bill goodenergy {87.38, y, m, b_d, false};
    
    // plusnet
    b_d = 12;
    Bill plusnet {30.0, y, m, b_d, false};
    
    // seven trent
    b_d = 16;
    Bill seventrent {10.0, y, m, b_d, false};
    
    temp.push_back(goodenergy);
    temp.push_back(plusnet);
    temp.push_back(seventrent);
    
    
    return temp;
}


/*
    Month addMonth();
    int getBillDayOfWeek();
    int getDayOfWeek();
    tm makeBillTM();
    vector<Bill> makeBills();
    plus
    int daysBetweenPay
    int daysUntilSafe

5 (7) helper functions
is there anything wrong?
seems like date, bill and analyser
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
    // the bills
    std::ifstream ifs{"bank.txt"};
    double sum;
    ifs >> sum;
    Date date{2025, Month::May, 1};
    vector<Bill> bills = makeBills(date);
    
    try
    {
        std::cout << "Date created: " << date << std::endl;
        std::cout << "Available: " << sum << std::endl;
    }
    catch(Date::Invalid)
    {
        cout << "Dates incorrect" << endl;
    }
    
    //
    // start it going
    //    
    
    try
    {
        	int count = 30;
        	while(count>0)
        	{
        	   	chrono::time_point<chrono::system_clock> now{chrono::system_clock::now()};
    	       t2 = chrono::system_clock::to_time_t(now);
	    
        	    // every second
        	    if(t2>t)
        	   	{
           	   		  //t = t2;
        	   	    //date.add_day();
        	   	    
        	   	    //for(int i=0; i < bills.size(); ++i)
        	   	    for(Bill &bill : bills)
        	   	    {
        	   	        if(date == bill)
        	   	       {
        	   	           // pay bill, move it to next month
        	   	           cout << "found bill " << bill << endl;
        	   	           sum -= bill.bill_me();
        	   	           cout << "£" << sum << endl;
        	   	           
        	   	           bill.restore_month_and_add();
        	   	       }
        	   	    }
        	   	    
        	   	    // find the bill on the day
        	   	    date.add_day();
       	   	     count--;
        	   	}
        	}
        
        for(Bill bill : bills)
        {
            cout << bill.bill_me() << " is now on " << bill << endl;
        }
        //cout << "goodenergy is " << bills[0] << endl;
        //cout << "plusnet is " << bills[1] << endl;
        //cout << "seventrent is " << bills[2] << endl;
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

Month addMonth(const Month& m)
{
    switch(m)
    {
    case Month::Jan: return Month::Feb;
    case Month::Feb: return Month::Mar;
    case Month::Mar: return Month::Apr;
    case Month::Apr: return Month::May;
    case Month::May: return Month::Jun;
    case Month::Jun: return Month::Jul;
    case Month::Jul: return Month::Aug;
    case Month::Aug: return Month::Sep;
    case Month::Sep: return Month::Oct;
    case Month::Oct: return Month::Nov;
    case Month::Nov: return Month::Dec;
    case Month::Dec: return Month::Jan;
    }
}

Month minusMonth(const Month& m)
{
    switch(m)
    {
    case Month::Mar: return Month::Feb;
    case Month::Apr: return Month::Mar;
    case Month::May: return Month::Apr;
    case Month::Jun: return Month::May;
    case Month::Jul: return Month::Jun;
    case Month::Aug: return Month::Jul;
    case Month::Sep: return Month::Aug;
    case Month::Oct: return Month::Sep;
    case Month::Nov: return Month::Oct;
    case Month::Dec: return Month::Nov;
    case Month::Jan: return Month::Dec;
    case Month::Feb: return Month::Jan;
    }
}

/*
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
*/
