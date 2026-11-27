#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ==========================================
// MODULE 1: Manage Product Catalogue
// ==========================================
class Product {
private:
    int id;
    string name;
    double price;
    int stock;

public:
    Product(int id, string name, double price, int stock)
        : id(id), name(name), price(price), stock(stock) {}

    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void setStock(int newStock) { stock = newStock; }

    bool reduceStock(int qty) {
        if (qty <= 0 || qty > stock) {
            return false;
        }
        stock -= qty;
        return true;
    }

    void restoreStock(int qty) {
        if (qty > 0) {
            stock += qty;
        }
    }
};

class ProductCatalog {
private:
    vector<Product> products;

public:
    void addProduct(int id, const string& name, double price, int stock) {
        if (findProductById(id) != nullptr) {
            cout << "Product ID already exists.\n";
            return;
        }
        products.emplace_back(id, name, price, stock);
        cout << "Product added to catalogue.\n";
    }

    Product* findProductById(int id) {
        for (auto& p : products) {
            if (p.getId() == id) {
                return &p;
            }
        }
        return nullptr;
    }

    void displayCatalogue() const {
        cout << "\n--- PRODUCT CATALOGUE ---\n";
        if (products.empty()) {
            cout << "No products available.\n";
            return;
        }
        cout << left << setw(6) << "ID"
             << setw(22) << "Name"
             << right << setw(10) << "Price"
             << setw(8) << "Stock" << "\n";
        cout << string(46, '-') << "\n";
        for (const auto& p : products) {
            cout << left << setw(6) << p.getId()
                 << setw(22) << p.getName()
                 << right << setw(10) << fixed << setprecision(2) << p.getPrice()
                 << setw(8) << p.getStock() << "\n";
        }
    }
};

// ==========================================
// MODULE 2: Add and Remove Items from Cart
// ==========================================
struct CartItem {
    int productId;
    string productName;
    double unitPrice;
    int quantity;

    double getLineTotal() const {
        return unitPrice * quantity;
    }
};

class ShoppingCart {
private:
    vector<CartItem> items;

public:
    bool addItem(ProductCatalog& catalog, int productId, int qty) {
        if (qty <= 0) {
            cout << "Quantity must be greater than zero.\n";
            return false;
        }

        Product* product = catalog.findProductById(productId);
        if (product == nullptr) {
            cout << "Product not found.\n";
            return false;
        }

        if (!product->reduceStock(qty)) {
            cout << "Insufficient stock. Available: " << product->getStock() << "\n";
            return false;
        }

        for (auto& item : items) {
            if (item.productId == productId) {
                item.quantity += qty;
                cout << "Updated cart quantity for " << product->getName() << ".\n";
                return true;
            }
        }

        items.push_back({product->getId(), product->getName(), product->getPrice(), qty});
        cout << "Added " << qty << " x " << product->getName() << " to cart.\n";
        return true;
    }

    bool removeItem(ProductCatalog& catalog, int productId, int qtyToRemove) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->productId == productId) {
                if (qtyToRemove <= 0 || qtyToRemove >= it->quantity) {
                    Product* product = catalog.findProductById(productId);
                    if (product != nullptr) {
                        product->restoreStock(it->quantity);
                    }
                    cout << "Removed " << it->productName << " from cart.\n";
                    items.erase(it);
                } else {
                    it->quantity -= qtyToRemove;
                    Product* product = catalog.findProductById(productId);
                    if (product != nullptr) {
                        product->restoreStock(qtyToRemove);
                    }
                    cout << "Reduced " << it->productName << " quantity by " << qtyToRemove << ".\n";
                }
                return true;
            }
        }
        cout << "Item not found in cart.\n";
        return false;
    }

    void viewCart() const {
        cout << "\n--- SHOPPING CART ---\n";
        if (items.empty()) {
            cout << "Cart is empty.\n";
            return;
        }
        cout << left << setw(6) << "ID"
             << setw(20) << "Item"
             << right << setw(8) << "Qty"
             << setw(10) << "Price"
             << setw(12) << "Total" << "\n";
        cout << string(56, '-') << "\n";
        for (const auto& item : items) {
            cout << left << setw(6) << item.productId
                 << setw(20) << item.productName
                 << right << setw(8) << item.quantity
                 << setw(10) << fixed << setprecision(2) << item.unitPrice
                 << setw(12) << fixed << setprecision(2) << item.getLineTotal() << "\n";
        }
    }

    const vector<CartItem>& getItems() const { return items; }
    bool isEmpty() const { return items.empty(); }
    void clear() { items.clear(); }
};

// ==========================================
// MODULE 3: Calculate Discounts, Tax & Total
// ==========================================
struct BillSummary {
    double subtotal = 0.0;
    double discountRate = 0.0;
    double discountAmount = 0.0;
    double taxableAmount = 0.0;
    double taxRate = 0.0;
    double taxAmount = 0.0;
    double grandTotal = 0.0;
};

class BillingCalculator {
private:
    double taxRate; // e.g., 0.18 for 18%

public:
    explicit BillingCalculator(double taxRate = 0.18) : taxRate(taxRate) {}

    BillSummary calculate(const ShoppingCart& cart) const {
        BillSummary summary;
        for (const auto& item : cart.getItems()) {
            summary.subtotal += item.getLineTotal();
        }

        // Tiered discount rule: 10% for subtotal >= 1000, 5% for >= 500, else 0%
        if (summary.subtotal >= 1000.0) {
            summary.discountRate = 0.10;
        } else if (summary.subtotal >= 500.0) {
            summary.discountRate = 0.05;
        } else {
            summary.discountRate = 0.0;
        }

        summary.discountAmount = summary.subtotal * summary.discountRate;
        summary.taxableAmount = summary.subtotal - summary.discountAmount;
        summary.taxRate = taxRate;
        summary.taxAmount = summary.taxableAmount * summary.taxRate;
        summary.grandTotal = summary.taxableAmount + summary.taxAmount;

        return summary;
    }
};

// ==========================================
// MODULE 4: Generate and Print Invoices
// ==========================================
class InvoicePrinter {
private:
    int invoiceCounter;

public:
    InvoicePrinter() : invoiceCounter(1001) {}

    void printInvoice(const ShoppingCart& cart, const BillSummary& bill) {
        if (cart.isEmpty()) {
            cout << "Cannot generate invoice for an empty cart.\n";
            return;
        }

        cout << "\n========================================\n";
        cout << "               TAX INVOICE              \n";
        cout << " Invoice No: INV-" << invoiceCounter++ << "\n";
        cout << "========================================\n";
        cout << left << setw(18) << "Item"
             << right << setw(5) << "Qty"
             << setw(9) << "Rate"
             << setw(10) << "Amount" << "\n";
        cout << string(42, '-') << "\n";

        for (const auto& item : cart.getItems()) {
            cout << left << setw(18) << item.productName.substr(0, 17)
                 << right << setw(5) << item.quantity
                 << setw(9) << fixed << setprecision(2) << item.unitPrice
                 << setw(10) << fixed << setprecision(2) << item.getLineTotal() << "\n";
        }

        cout << string(42, '-') << "\n";
        cout << left << setw(32) << "Subtotal:"
             << right << setw(10) << fixed << setprecision(2) << bill.subtotal << "\n";
        cout << left << setw(32) << ("Discount (" + to_string(static_cast<int>(bill.discountRate * 100)) + "%):")
             << right << setw(10) << fixed << setprecision(2) << -bill.discountAmount << "\n";
        cout << left << setw(32) << ("Tax (" + to_string(static_cast<int>(bill.taxRate * 100)) + "%):")
             << right << setw(10) << fixed << setprecision(2) << bill.taxAmount << "\n";
        cout << string(42, '=') << "\n";
        cout << left << setw(32) << "TOTAL PAYABLE:"
             << right << setw(10) << fixed << setprecision(2) << bill.grandTotal << "\n";
        cout << "========================================\n";
        cout << "        Thank you for shopping!         \n\n";
    }
};

// ==========================================
// Application Entry Point
// ==========================================
void seedSampleProducts(ProductCatalog& catalog) {
    catalog.addProduct(101, "USB Mouse", 450.00, 15);
    catalog.addProduct(102, "Mechanical Keyboard", 1800.00, 10);
    catalog.addProduct(103, "HDMI Cable", 250.00, 25);
    catalog.addProduct(104, "Laptop Stand", 950.00, 8);
}

int main() {
    ProductCatalog catalog;
    ShoppingCart cart;
    BillingCalculator billing(0.18); // 18% tax
    InvoicePrinter printer;

    seedSampleProducts(catalog);

    int choice = -1;
    while (choice != 0) {
        cout << "\n=== SHOPPING & BILLING SYSTEM ===\n";
        cout << "1. View Catalogue\n";
        cout << "2. Add Product to Catalogue\n";
        cout << "3. Add Item to Cart\n";
        cout << "4. Remove Item from Cart\n";
        cout << "5. View Cart\n";
        cout << "6. Checkout & Print Invoice\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            catalog.displayCatalogue();
        } else if (choice == 2) {
            int id, stock;
            double price;
            string name;
            cout << "Enter ID: ";
            cin >> id;
            cin.ignore();
            cout << "Enter Name: ";
            getline(cin, name);
            cout << "Enter Price: ";
            cin >> price;
            cout << "Enter Stock: ";
            cin >> stock;
            catalog.addProduct(id, name, price, stock);
        } else if (choice == 3) {
            int id, qty;
            catalog.displayCatalogue();
            cout << "Enter Product ID to add: ";
            cin >> id;
            cout << "Enter Quantity: ";
            cin >> qty;
            cart.addItem(catalog, id, qty);
        } else if (choice == 4) {
            int id, qty;
            cart.viewCart();
            cout << "Enter Product ID to remove: ";
            cin >> id;
            cout << "Enter Quantity to remove: ";
            cin >> qty;
            cart.removeItem(catalog, id, qty);
        } else if (choice == 5) {
            cart.viewCart();
        } else if (choice == 6) {
            if (cart.isEmpty()) {
                cout << "Your cart is empty. Add items before checkout.\n";
            } else {
                BillSummary summary = billing.calculate(cart);
                printer.printInvoice(cart, summary);
                cart.clear();
            }
        } else if (choice == 0) {
            cout << "Exiting system. Goodbye!\n";
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}