#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <cctype>
using namespace std;

class User {
private:
    string username;
    string password;
    string licenseKey;
    bool verified;
    int orderCount;
    bool isSeller;  // New field to identify sellers

public:
    User(string uname, string pwd, string license, bool seller = false) 
        : username(uname), password(pwd), licenseKey(license), verified(false), orderCount(0), isSeller(seller) {}

    bool verifyLicense(string inputKey) {
        if (inputKey == licenseKey) {
            verified = true;
            return true;
        }
        return false;
    }

    bool isVerified() const { return verified; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    int getOrderCount() const { return orderCount; }
    bool getIsSeller() const { return isSeller; }  // Getter for seller status
    void incrementOrderCount() { orderCount++; }
};

class Product {
protected:
    string name;
    map<string, double> countryPrices;
    string category;

public:
    Product(string n, map<string, double> prices, string cat) 
        : name(n), countryPrices(prices), category(cat) {}
    
    virtual ~Product() {}

    virtual double calculatePrice(string country, double tariff, bool landlocked) const {
        double price = countryPrices.at(country) * (1 + tariff);
        if (landlocked) price *= 1.05;
        return price;
    }

    string getName() const { return name; }
    string getCategory() const { return category; }
    double getBasePrice(string country) const { 
        return countryPrices.at(country); 
    }
    bool isAvailableIn(string country) const {
        return countryPrices.find(country) != countryPrices.end();
    }
};

class Electronics : public Product {
private:
    string size;

public:
    Electronics(string n, map<string, double> prices, string cat, string sz)
        : Product(n, prices, cat), size(sz) {}

    string getSize() const { return size; }
};

class CountryManager {
private:
    map<string, double> tariffs = {
        {"USA", 0.15}, {"China", 0.10}, 
        {"Iran", 0.10}, {"Japan", 0.12}
    };

    vector<string> landlocked = {"Iran"};
    vector<string> countries = {"USA", "China", "Iran", "Japan"};

public:
    double getTariff(string country) const {
        auto it = tariffs.find(country);
        if (it != tariffs.end()) return it->second;
        throw invalid_argument("Country not found");
    }

    bool isLandlocked(string country) const {
        return find(landlocked.begin(), landlocked.end(), country) != landlocked.end();
    }

    const vector<string>& getCountries() const { return countries; }
};

class Order {
private:
    vector<pair<Product*, string>> items;
    static const int MAX_ITEMS = 3;

public:
    Order() {}

    bool addItem(Product* product, string country) {
        if (items.size() < MAX_ITEMS && product->isAvailableIn(country)) {
            items.push_back({product, country});
            return true;
        }
        return false;
    }

    double display(const CountryManager& cm) const {
        cout << "\n--- Your Order ---\n";
        double total = 0.0;
        double profit = 0.0;
        
        for (const auto& item : items) {
            Product* product = item.first;
            string country = item.second;
            double tariff = cm.getTariff(country);
            bool landlocked = cm.isLandlocked(country);
            double finalPrice = product->calculatePrice(country, tariff, landlocked);
            double basePrice = product->getBasePrice(country);
            
            cout << product->getName();
            if (auto e = dynamic_cast<const Electronics*>(product)) {
                cout << " (" << e->getSize() << ")";
            }
            cout << " from " << country
                 << ": Base $" << fixed << setprecision(2) << basePrice
                 << ", Final $" << finalPrice << endl;
            
            total += finalPrice;
            profit += (finalPrice - basePrice);
        }
        cout << "Total: $" << total << endl;
        cout << "Business Profit: $" << profit << endl;
        return profit;
    }

    bool isFull() const { return items.size() >= MAX_ITEMS; }
};

class ProductCatalog {
public:
    static vector<Product*>& getElectronics() {
        static vector<Product*> electronics = {
            new Electronics("Fridge", 
                {{"USA", 800}, {"China", 750}, {"Iran", 820}, {"Japan", 850}}, 
                "Electronics", "Standard"),
            new Electronics("Oven", 
                {{"USA", 450}, {"China", 400}, {"Iran", 480}, {"Japan", 470}}, 
                "Electronics", "Standard"),
            new Electronics("Vacuum Cleaner", 
                {{"USA", 150}, {"China", 120}, {"Iran", 160}, {"Japan", 155}}, 
                "Electronics", "Standard"),
            new Electronics("Display", 
                {{"USA", 300}, {"China", 250}, {"Iran", 320}, {"Japan", 310}}, 
                "Electronics", "32 inches"),
            new Electronics("Display", 
                {{"USA", 1200}, {"China", 1100}, {"Iran", 1250}, {"Japan", 1220}}, 
                "Electronics", "72 inches")
        };
        return electronics;
    }

    static vector<Product*>& getStationary() {
        static vector<Product*> stationary = {
            new Product("Notebook Set", 
                {{"USA", 15}, {"China", 12}, {"Iran", 16}, {"Japan", 14}}, 
                "Stationary"),
            new Product("Pen Pack", 
                {{"USA", 8}, {"China", 5}, {"Iran", 9}, {"Japan", 8}}, 
                "Stationary"),
            new Product("Art Supplies", 
                {{"USA", 35}, {"China", 30}, {"Iran", 38}, {"Japan", 36}}, 
                "Stationary"),
            new Product("Office Organizer", 
                {{"USA", 45}, {"China", 40}, {"Iran", 48}, {"Japan", 46}}, 
                "Stationary")
        };
        return stationary;
    }

    static vector<Product*>& getHomeUsage() {
        static vector<Product*> homeUsage = {
            new Product("Kitchen Set", 
                {{"USA", 120}, {"China", 100}, {"Iran", 130}, {"Japan", 125}}, 
                "Home Usage"),
            new Product("Bedding", 
                {{"USA", 85}, {"China", 70}, {"Iran", 90}, {"Japan", 88}}, 
                "Home Usage"),
            new Product("Furniture", 
                {{"USA", 300}, {"China", 250}, {"Iran", 320}, {"Japan", 310}}, 
                "Home Usage"),
            new Product("Decorations", 
                {{"USA", 65}, {"China", 50}, {"Iran", 70}, {"Japan", 68}}, 
                "Home Usage")
        };
        return homeUsage;
    }

    static vector<Product*>& getAutomobile() {
        static vector<Product*> automobile = {
            new Product("Sedan", 
                {{"USA", 15000}, {"China", 14000}, {"Iran", 15500}, {"Japan", 15200}}, 
                "Automobile"),
            new Product("SUV", 
                {{"USA", 35000}, {"China", 32000}, {"Iran", 36000}, {"Japan", 35500}}, 
                "Automobile"),
            new Product("Hatchback", 
                {{"USA", 12000}, {"China", 10000}, {"Iran", 12500}, {"Japan", 12200}}, 
                "Automobile"),
            new Product("Electric Car", 
                {{"USA", 45000}, {"China", 42000}, {"Iran", 46000}, {"Japan", 45500}}, 
                "Automobile")
        };
        return automobile;
    }

    static void cleanup() {
        for (auto p : getElectronics()) delete p;
        for (auto p : getStationary()) delete p;
        for (auto p : getHomeUsage()) delete p;
        for (auto p : getAutomobile()) delete p;
    }
};

class Application {
private:
    vector<User> users;
    CountryManager countryManager;
    User* currentUser = nullptr;
    double totalBusinessProfit;

    void registerUser() {
        string username, password, license;
        cout << "\n--- Registration ---\n";
        cout << "Username: "; 
        cin >> username;
        
        for (const auto& user : users) {
            if (user.getUsername() == username) {
                cout << "Username already exists!\n";
                return;
            }
        }
        
        cout << "Password: "; 
        cin >> password;
        cout << "License Key: "; 
        cin >> license;
        
        // Ask if user is registering as seller
        char sellerChoice;
        cout << "Are you registering as a seller? (y/n): ";
        cin >> sellerChoice;
        bool isSeller = (sellerChoice == 'y' || sellerChoice == 'Y');
        
        users.emplace_back(username, password, license, isSeller);
        cout << "Registration successful! Please verify your license.\n";
    }

    void registerSeller() {
        string username, password, license;
        cout << "\n--- Seller Registration ---\n";
        cout << "Username: "; 
        cin >> username;
        
        for (const auto& user : users) {
            if (user.getUsername() == username) {
                cout << "Username already exists!\n";
                return;
            }
        }
        
        cout << "Password: "; 
        cin >> password;
        cout << "License Key: "; 
        cin >> license;
        
        users.emplace_back(username, password, license, true); // true for isSeller
        cout << "Seller registration successful! Please verify your license.\n";
    }

    void verifyLicense() {
        if (!currentUser) {
            cout << "No user logged in!\n";
            return;
        }
        
        string license;
        cout << "Enter license key: ";
        cin >> license;
        
        if (currentUser->verifyLicense(license)) {
            cout << "Verification successful!\n";
        } else {
            cout << "Invalid license key!\n";
        }
    }

    void login() {
        string username, password;
        cout << "\n--- Login ---\n";
        cout << "Username: "; 
        cin >> username;
        cout << "Password: "; 
        cin >> password;
        
        currentUser = nullptr;
        for (auto& user : users) {
            if (user.getUsername() == username && user.getPassword() == password) {
                currentUser = &user;
                break;
            }
        }
        
        if (!currentUser) {
            cout << "Invalid username or password!\n";
            return;
        }
        
        if (!currentUser->isVerified()) {
            cout << "Account not verified! ";
            verifyLicense();
        }
    }

    void displayProductsByCategory(const vector<Product*>& products) {
        for (size_t i = 0; i < products.size(); ++i) {
            cout << i + 1 << ". " << products[i]->getName();
            if (auto e = dynamic_cast<Electronics*>(products[i])) {
                cout << " (" << e->getSize() << ")";
            }
            cout << " - Category: " << products[i]->getCategory() << endl;
            
            for (const auto& country : countryManager.getCountries()) {
                if (products[i]->isAvailableIn(country)) {
                    cout << "   - " << country << ": $" 
                         << products[i]->getBasePrice(country) << endl;
                }
            }
        }
    }

    void orderProducts() {
        if (!currentUser || !currentUser->isVerified()) {
            cout << "Please login with verified account first!\n";
            return;
        }
        
        if (currentUser->getOrderCount() >= 3) {
            cout << "You've reached your order limit (3 orders per user)!\n";
            return;
        }

        Order order;
        vector<vector<Product*>> categories = {
            ProductCatalog::getElectronics(),
            ProductCatalog::getStationary(),
            ProductCatalog::getHomeUsage(),
            ProductCatalog::getAutomobile()
        };

        string categoryNames[] = {
            "Electronics", "Stationary", "Home Usage", "Automobile"
        };

        while (!order.isFull()) {
            cout << "\n--- Product Catalog ---\n";
            for (int i = 0; i < 4; i++) {
                cout << i + 1 << ". " << categoryNames[i] << endl;
            }
            cout << "5. Checkout\n";
            cout << "Choose category: ";

            int choice;
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }

            if (choice == 5) break;
            if (choice < 1 || choice > 4) {
                cout << "Invalid choice!\n";
                continue;
            }

            vector<Product*>& products = categories[choice - 1];
            displayProductsByCategory(products);
            cout << "Select product: ";
            
            int productChoice;
            cin >> productChoice;
            
            if (productChoice < 1 || productChoice > static_cast<int>(products.size())) {
                cout << "Invalid selection!\n";
                continue;
            }

            Product* selected = products[productChoice - 1];
            
            cout << "Available countries: ";
            for (const auto& country : countryManager.getCountries()) {
                if (selected->isAvailableIn(country)) {
                    cout << country << " ";
                }
            }
            cout << "\nSelect country: ";
            string country;
            cin >> country;
            
            bool validCountry = false;
            for (const auto& c : countryManager.getCountries()) {
                if (c == country) {
                    validCountry = true;
                    break;
                }
            }
            
            if (!validCountry) {
                cout << "Invalid country selection!\n";
                continue;
            }
            
            if (order.addItem(selected, country)) {
                cout << "Added to order!\n";
            } else {
                cout << "Order full! Max 3 items.\n";
                break;
            }
        }
        
        double profit = order.display(countryManager);
        totalBusinessProfit += profit;
        currentUser->incrementOrderCount();
        cout << "Order placed successfully!\n";
        
        currentUser = nullptr;
        cout << "Logged out. Please login again for next order.\n";
    }

    void checkUserLicense() {
        if (!currentUser || !currentUser->isVerified()) {
            cout << "Please login with verified account first!\n";
            return;
        }
        
        string username;
        cout << "Enter username to check: ";
        cin >> username;
        
        bool found = false;
        for (const auto& user : users) {
            if (user.getUsername() == username) {
                found = true;
                cout << "User: " << username << "\n";
                cout << "License Status: " << (user.isVerified() ? "Verified" : "Not Verified") << "\n";
                cout << "User Type: " << (user.getIsSeller() ? "Seller" : "Customer") << "\n";
                cout << "Orders Placed: " << user.getOrderCount() << "/3\n";
                break;
            }
        }
        
        if (!found) {
            cout << "User not found!\n";
        }
    }

    void showBusinessProfit() {
        if (!currentUser || !currentUser->isVerified()) {
            cout << "Please login with verified account first!\n";
            return;
        }
        
        // Check if user is a seller
        if (!currentUser->getIsSeller()) {
            cout << "Access denied! Only sellers can view business profit.\n";
            return;
        }
        
        cout << "\n--- Business Profit Report ---\n";
        cout << "Total Profit: $" << fixed << setprecision(2) << totalBusinessProfit << "\n";
    }

    void sellerDashboard() {
        if (!currentUser || !currentUser->isVerified()) {
            cout << "Please login with verified account first!\n";
            return;
        }
        
        if (!currentUser->getIsSeller()) {
            cout << "Access denied! Only sellers can access this dashboard.\n";
            return;
        }
        
        cout << "\n--- Seller Dashboard ---\n";
        cout << "1. View Business Profit\n";
        cout << "2. Back to Main Menu\n";
        cout << "Choose option: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                showBusinessProfit();
                break;
            case 2:
                return;
            default:
                cout << "Invalid choice!\n";
        }
    }

public:
    Application() : totalBusinessProfit(0.0) {}

    void run() {
        while (true) {
            cout << "\n--- Cross-Trade Logistics Platform ---\n";
            cout << "1. Register\n2. Login\n3. Order Products\n4. Check User License\n5. Seller Dashboard\n6. Exit\n";
            cout << "Choose option: ";
            
            int choice;
            cin >> choice;
            
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }

            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2:
                    login();
                    break;
                case 3:
                    orderProducts();
                    break;
                case 4:
                    checkUserLicense();
                    break;
                case 5:
                    sellerDashboard();
                    break;
                case 6:
                    cout << "Exiting...\n";
                    return;
                default:
                    cout << "Invalid choice!\n";
            }
        }
    }
};

int main() {
    Application app;
    app.run();
    ProductCatalog::cleanup();
    return 0;
}