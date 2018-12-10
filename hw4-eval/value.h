/*
 * value.h

 class ParseTree {
    int         linenum;
    ParseTree   *left;
    ParseTree   *right;
 */

#ifndef VALUE_H_
#define VALUE_H_

#include <string>
#include <iostream>
using namespace std;

extern void RunTimeError (string);

// object holds boolean, integer, or string, and remembers which it holds
class Value {
    bool    bval;
    int     ival;
    string  sval;
    enum VT { isBool, isInt, isString, isTypeError } type;
    //enum somehwere else, just retrun type

public:
    Value() : bval(false), ival(0), type(isTypeError) {}
    Value(bool bval) : bval(bval), ival(0), type(isBool) {}
    Value(int ival) : bval(false), ival(ival), type(isInt) {}
    Value(string sval) : bval(false), ival(0), sval(sval), type(isString) {}

    // in the case of an error, I use the value to hold the error message
    Value(string sval, bool isError) : bval(false), ival(0), sval(sval), type(isTypeError) {}

    Value GetType() const { return type; } //generic for each type
    bool isBoolType() const { return type == VT::isBool; }
    bool isIntType() const { return type == VT::isInt; }
    bool isStringType() const { return type == VT::isString; }
    bool isError() const { return type == VT::isTypeError; }
    bool hasMessage() const { return isError() && sval.size() > 0; }

    bool isTrue() const { return isBoolType() && bval; }
    bool getBoolean() const {
        if( !isBoolType() )
            throw "Not boolean valued";
        return bval;
    }

    int getInteger() const {
        if( !isIntType() )
            throw "Not integer valued";
        return ival;
    }

    string getString() const {
        if( !isStringType() )
            throw "Not string valued";
        return sval;
    }

    string getMessage() const {
        if( !hasMessage() )
            throw "No message";
        return sval;
    }
    //https://www.tutorialspoint.com/cplusplus/cpp_friend_functions.htm
    friend ostream& operator<<(ostream& out, const Value& v) {
        if( v.type == VT::isBool ) out << (v.bval ? "True" : "False");
        else if( v.type == VT::isInt ) out << v.ival;
        else if( v.type == VT::isString ) out << v.sval;
        else if( v.sval.size() > 0 ) out << "RUNTIME ERROR " << v.sval;
        else out << "TYPE ERROR";
        return out;
    }

    //Operator +
    Value operator+(const Value& v) {
        if(this->isIntType() && v.isIntType()) {
          int result = this->getInteger() + v.getInteger();
          return result;
        }
        if(this->isStringType() && v.isStringType()) {
            string result = this->getString() + v.getString();
            return result;
        } else {
            RunTimeError("Cannot add these two values");
            return Value();
        }   
    }

    Value operator-(const Value& v) {
        if(this->isIntType() && v.isIntType()) {
          int result = this->getInteger() - v.getInteger();
          return result;
        }
        if(this->isStringType() && v.isStringType()) {
            string result = this->getString() + v.getString();
            return result;
        } else {
            RunTimeError("Cannot subtract these two values");
            return Value();
        }   
    }
    
    Value operator*(const Value& v) {  
        //2*2
        if(this->isIntType() && v.isIntType()) {
           return ival * v.ival;
        }
        //2*hello - hellohello
        else if(this->isIntType() && v.isStringType()) {
            /*
            hello*2; If +, loop append to string
            */
            if(this->getInteger() >= 0) {
                string addexpr;
                for(int i = 0; i < this->getInteger(); i++) {
                    addexpr = addexpr + v.getString();
                }
                    return addexpr;
                } else if(this->isIntType() && v.isBoolType()) {
                    if(ival == -1) {
                        return Value(false);
                    } 
                    if(ival == 1) {
                        //bool response = !v.getBoolean();
                        return Value(true);
                    }
                } else {
                    RunTimeError("Cannot multiply a string by a negative integer");
                    return Value();
                }
        }
        //hello*2
        //hellohello
        else if(this->isStringType() && v.isIntType()) {
            /*
            multstring by a positive number: hello*2 exclude hello*-2
            */
            if(v.getInteger() >= 0) {//block -ints from strings
                string addexpr = "";
                for(int i = 0; i < v.getInteger(); i++)
                {
                 addexpr = addexpr + this->getString();//
                }
                return addexpr;
            } else {
                RunTimeError("Cannot multiply a string by a negative integer");
                return Value();  
            }
        } else if(this->isIntType() && v.isBoolType()) {
            if(ival == -1) {
                bool result = !v.getBoolean(); //- flips booleans
                return Value(result);
            } 
            if(ival == 1) {
                return Value(true);
            }
         } else {
            RunTimeError("Cannot multiply these two values");
            return Value();
        }
        return Value();
    }

    Value operator/(const Value& v) {
        if(v.isIntType() && v.getInteger() == 0) {
            RunTimeError("Division by 0"); //catch 0 divisions
            return Value();
        } else if(this->isIntType() && v.isIntType()) {
            int result = this->getInteger() / v.getInteger();
            return Value(result);
        } else {
            RunTimeError("Cannot divide these two values");
            return Value();
        }   
    }

    Value operator<(const Value& v) {
        if(this->isIntType() && v.isIntType()) {
            bool result = this->getInteger() < v.getInteger();
            return result;
        } else if (this->isStringType() && v.isStringType()) {
            bool result = this->getString() < v.getString();
            return result;
        } else {
            RunTimeError("Type mismatch in <");
            return Value();
        }
    }

    Value operator<=(const Value& v) {
        if(this->isIntType() && v.isIntType()) {
            bool result = this->getInteger() <= v.getInteger();
            return result;
        } else if(this->isStringType() && v.isStringType()) {
            bool result = this->getString() <= v.getString();
            return result;  
        } else {
            RunTimeError("Type mismatch in <=");
            return Value();
        }
    }

    Value operator>(const Value& v) {
        if(this->isIntType() && v.isIntType()) {
            bool result = this->getInteger() > v.getInteger();
            return result;
        } else if(this->isStringType() && v.isStringType()) {
            bool result = this->getString() > v.getString();
            return result;  
        } else {
            RunTimeError("Type mismatch in >");
            return Value();
        }
    }

    Value operator>=(const Value& v) {
         if(this->isIntType() && v.isIntType()) {
            bool result = this->getInteger() >= v.getInteger();
            return result;
        } else if(this->isStringType() && v.isStringType()) {
            bool result = this->getString() >= v.getString();
            return result;  
        } else {
            RunTimeError("Type mismatch in >=");
            return Value();  
        }      
    }

    Value operator==(const Value& v) {
        if(this->isIntType() && v.isIntType()) {
            bool result = this->getInteger() == v.getInteger();
            return result;
        } else if(this->isStringType() && v.isStringType()) {
            bool result = this->getString() == v.getString();
            return result;  
        } else if(this->isBoolType() && v.isBoolType()) {
            bool result = this->getBoolean() == v.getBoolean();
            return result;  
        } else {
            RunTimeError("Type mismatch in ==");
            return Value();    
        }
    }

    Value operator!=(const Value& v) {
        Value ans = this->operator==(v);
        ans.bval = !ans.bval;
        return ans;
    }
    
};

#endif /* VALUE_H_ */
