#pragma once

#include <vector>
#include <string>
#include <iostream>

class ParserOperation {
public:
    std::string parameter_name;
};

class Transition {
public:
    std::string value;
    std::string mask;
    std::string next_state;
};

class TransitionKey {
public:
    std::vector<std::string> value;
};

class P4ParseState {
public:
    std::string name;
    std::vector<ParserOperation> parser_ops;
    std::vector<Transition> transitions;
    std::vector<TransitionKey> transition_key;    
};

class P4Parser {
public:
    std::string name;
    std::string init_state;
    std::vector<P4ParseState> parse_states;
};

class P4Parsers : public std::vector<P4Parser> {
    friend std::ostream & operator<<(std::ostream & out, P4Parsers const & vp);
};

std::ostream & operator<<(std::ostream & out, P4Parsers const & vp) {
    for (auto & p : vp) {
        out << "parser " << (p.name == "parser" ? "MyParser" : p.name) << "(packet_in packet) {" << std::endl;
        for (auto & ps : p.parse_states) {
            out << "\tstate " << ps.name << " {" << std::endl;
            for (auto & op : ps.parser_ops) {
                out << "\t\tpacket.extract(hdr." << op.parameter_name << ");" << std::endl;
            }
            if (ps.transition_key.size() > 0) {
                out << "\t\ttransition select(";
                for (bool first = true; auto& tk : ps.transition_key) {
                    if (!first) {
                        out << ", ";
                    } else {
                        first = false;
                    }
                    out << "hdr";
                    for (auto & v : tk.value) {
                        out << "." << v;
                    }
                }
                out << ") {" << std::endl;
                for (auto & tr : ps.transitions) {
                    out << "\t\t\t" << tr.value;
                    if (tr.mask.size() > 0) {
                        out << " &&& " << tr.mask;
                    }
                    out << ": ";
                    if (tr.next_state.size() > 0) {
                        out << tr.next_state;
                    } else {
                        out << "accept";
                    }
                    out << ";" << std::endl;
                }
                out << "\t\t}" << std::endl;
            } else {
                out << "\t\ttransition ";
                for (auto & tr : ps.transitions) {
                    if (tr.next_state.size() > 0) {
                        out << tr.next_state;
                    } else {
                        out << "accept";
                    }
                    out << ";" << std::endl;
                }
            }
            out << "\t}" << std::endl;
        }
        out << "}" << std::endl;
    }
    return out;
}