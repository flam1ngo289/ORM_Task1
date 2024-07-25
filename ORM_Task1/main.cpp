#include <iostream>
#include <WT/Dbo/Dbo.h>
#include <WT/Dbo/backend/Postgres.h>
#include <Windows.h>
#include <memory>

using namespace std;

#pragma execution_character_set("utf-8")

class Book;
class Shop;
class Publisher;
class Stock;
class Sale;

class Publisher {
public:
	string name = "";
	Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");
	}
};

class Book {
public:
	string title = "";
	Wt::Dbo::ptr<Publisher> publisher;
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::belongsTo(a, publisher, "publisher");  //дописывает _id в таблице
		Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "book");
	}
};

class Stock {
public:
	int count{};
	Wt::Dbo::ptr<Shop> shops;
	Wt::Dbo::ptr<Book> books;
	Wt::Dbo::collection<Wt::Dbo::ptr<Sale>> sales;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, shops, "shop");
		Wt::Dbo::belongsTo(a, books, "book");
		Wt::Dbo::hasMany(a, sales, Wt::Dbo::ManyToOne, "stock");
	}
};

class Shop {
public:
	string name = "";
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stocks;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, stocks, Wt::Dbo::ManyToOne, "shop");
	}
};

class Sale {
public:
	int price{};
	string date_sale = "";
	int count{};
	Wt::Dbo::ptr<Stock> stocks;

	template<class Action>
	void persist(Action& a) {
		Wt::Dbo::field(a, date_sale, "date");
		Wt::Dbo::field(a, price, "price");
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, stocks, "stock");
	}
};

int main() {

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 10000);
	//setlocale(LC_ALL, "Russian");
	try {
		string connectionString =
			"host = localhost "
			"port = 5432 "
			"dbname = shop "
			"user = postgres "
			"password = postgres";

		unique_ptr<Wt::Dbo::backend::Postgres> connection = make_unique<Wt::Dbo::backend::Postgres>(connectionString);
		Wt::Dbo::Session s;


		s.setConnection(move(connection));
		s.mapClass<Publisher>("publisher");
		s.mapClass<Book>("book");
		s.mapClass<Sale>("sale");
		s.mapClass<Shop>("shop");
		s.mapClass<Stock>("stock");

		s.dropTables();
		s.createTables();
		Wt::Dbo::Transaction t(s);

		// издательство
		std::unique_ptr<Publisher> p1(new Publisher{ "ACT", {} }); //установка значений
		std::unique_ptr<Publisher> p2(new Publisher{ "Prosvet", {} });
		std::unique_ptr<Publisher> p3(new Publisher{ "Republic", {} });

		Wt::Dbo::ptr<Publisher>  p1db = s.add(std::move(p1)); // добавление в БД
		Wt::Dbo::ptr<Publisher>  p2db = s.add(std::move(p2));
		Wt::Dbo::ptr<Publisher>  p3db = s.add(std::move(p3));

		//магазин
		std::unique_ptr<Shop> shop1(new Shop{ "Litres", {} });
		std::unique_ptr<Shop> shop2(new Shop{ "Chitay", {} });
		std::unique_ptr<Shop> shop3(new Shop{ "Booker", {} });

		Wt::Dbo::ptr<Shop>  shop1db = s.add(std::move(shop1));
		Wt::Dbo::ptr<Shop>  shop2db = s.add(std::move(shop2));
		Wt::Dbo::ptr<Shop>  shop3db = s.add(std::move(shop3));

		//Книги в базе book
		std::unique_ptr<Book> book1{ new Book };
		book1->title = "Witcher";
		book1->publisher = p1db;
		std::unique_ptr<Book> book2(new Book{ "Alan Wake", p2db, {} });
		std::unique_ptr<Book> book3(new Book{ "Assassins", p3db, {} });

		Wt::Dbo::ptr<Book>  book1db = s.add(std::move(book1));
		Wt::Dbo::ptr<Book>  book2db = s.add(std::move(book2));
		Wt::Dbo::ptr<Book>  book3db = s.add(std::move(book3));

		//Stock
		std::unique_ptr<Stock> stock1(new Stock{ 10, shop1db, book1db, {} });
		std::unique_ptr<Stock> stock2(new Stock{ 10, shop2db, book2db, {} });
		std::unique_ptr<Stock> stock3(new Stock{ 10, shop3db, book3db, {} });
		std::unique_ptr<Stock> stock4(new Stock{ 10, shop1db, book3db, {} });

		Wt::Dbo::ptr<Stock>  stock1db = s.add(std::move(stock1));
		Wt::Dbo::ptr<Stock>  stock2db = s.add(std::move(stock2));
		Wt::Dbo::ptr<Stock>  stock3db = s.add(std::move(stock3));
		Wt::Dbo::ptr<Stock>  stock4db = s.add(std::move(stock4));


		//Sale
		std::unique_ptr<Sale> sale1(new Sale{ 22, "2024-07-10", 4, stock1db });
		std::unique_ptr<Sale> sale2(new Sale{ 20, "2024-06-24", 1, stock2db });
		std::unique_ptr<Sale> sale3(new Sale{ 19, "2024-02-13", 2, stock3db });

		Wt::Dbo::ptr<Sale>  sale1db = s.add(std::move(sale1));
		Wt::Dbo::ptr<Sale>  sale2db = s.add(std::move(sale2));
		Wt::Dbo::ptr<Sale>  sale3db = s.add(std::move(sale3));

		t.commit();

		string x{};
		int id{};
		Wt::Dbo::Transaction t1(s);
		Wt::Dbo::ptr<Publisher> pub;

		cout << "Введите имя или индефикатор" << endl;
		cin >> x;
		if (!x.empty() && (x.find_first_not_of("0123456779") == std::string::npos)) {
			pub = s.find<Publisher>().where("id=?").bind(x);
		}
		else {
			pub = s.find<Publisher>().where("name=?").bind(x);
		}

		if (pub) {
			cout << "Издатель:" << pub->name << endl;

			cout << "Списко магазинов, где продают книги издателя:" << pub->name << endl;

			std::set<std::string> shopNames;

			for (const auto& book : pub->books) {
				for (const auto& stock : book->stocks) {
					shopNames.insert(stock->shops->name);

				}
			}
			for (const auto& sh : shopNames)
				cout << sh << endl;

		}

	}
	catch (const exception& err) {
		cout << "Ошибка: " << err.what() << endl;
	}

	return 0;
}