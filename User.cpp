#include "User.hpp"

User::User(std::string name, std::string fullname, int id) : name(name), fullname(fullname), id(id) {}

std::string User::getName() {
	return name;
}

std::string User::getFullName() {
	return fullname;
}

int			User::getId() {
	return id;
}

bool operator<(User &c1, User &c2) {
	return c1.getId() < c2.getId();
}
std::ostream& operator<<(std::ostream &os, User &c) {
	os << c.getName();
}