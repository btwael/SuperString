#include "SuperString.hh"

/*-- imports --*/

// std
#include <iostream> // The only thing we need, and just for printing
#include <SuperString.hh>

/*-- definitions --*/

//*-- SuperString
SuperString::SuperString()
        : _sequence(NULL) {
    // nothing go here
}

SuperString::SuperString(const SuperString &other) /*copy*/ {
    this->_sequence = other._sequence;
    this->_sequence->refAdd();
}

SuperString::SuperString(SuperString::StringSequence *sequence)
        : _sequence(sequence) {
    this->_sequence->refAdd();
}

SuperString::~SuperString() {
    if(this->_sequence->refRelease() == 0) {
        delete this->_sequence;
    }
}

SuperString::Size SuperString::length() const {
    if(this->_sequence != NULL) {
        return this->_sequence->length();
    }
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::codeUnitAt(SuperString::Size index) const {
    if(this->_sequence != NULL) {
        return this->_sequence->codeUnitAt(index);
    }
    return Result<int, SuperString::Error>(Error::Unexpected);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::substring(SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->_sequence != NULL) {
        return this->_sequence->substring(startIndex, endIndex);
    }
    return Result<SuperString, Error>(Error::Unexpected);
}

void SuperString::print(std::ostream &stream) const {
    if(this->_sequence != NULL) {
        this->_sequence->print(stream);
    }
}

void SuperString::print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->_sequence != NULL) {
        this->_sequence->print(stream, startIndex, endIndex);
    }
}

SuperString SuperString::trim() const {
    if(this->_sequence != NULL) {
        return this->_sequence->trim();
    }
    return *this;
}

SuperString SuperString::trimLeft() const {
    if(this->_sequence != NULL) {
        return this->_sequence->trimLeft();
    }
    return *this;
}

SuperString SuperString::trimRight() const {
    if(this->_sequence != NULL) {
        return this->_sequence->trimRight();
    }
    return *this;
}

SuperString SuperString::operator+(const SuperString &other) const {
    ConcatenationSequence *sequence = new ConcatenationSequence(this->_sequence, other._sequence);
    return SuperString(sequence);
}

SuperString SuperString::operator*(Size times) const {
    MultipleSequence *sequence = new MultipleSequence(this->_sequence, times);
    return SuperString(sequence);
}

SuperString &SuperString::operator=(const SuperString &other) {
    if(this != &other) {
        if(this->_sequence->refRelease() == 0) {
            delete this->_sequence;
        }
        this->_sequence = other._sequence;
        this->_sequence->refAdd();
    }
    return *this;
}

SuperString SuperString::Const(const char *chars) {
    return SuperString(new SuperString::ConstASCIISequence(chars));
}

//*-- SuperString::StringSequence (abstract|internal)
SuperString::StringSequence::~StringSequence() {
    // nothing go here
}

SuperString::Bool SuperString::StringSequence::isEmpty() const {
    return this->length() == 0;
}

SuperString::Bool SuperString::StringSequence::isNotEmpty() const {
    return this->length() > 0;
}

void SuperString::StringSequence::refAdd() {
    this->_refCount++;
}

SuperString::Size SuperString::StringSequence::refRelease() {
    return this->_refCount--;
}

//*-- SuperString::ReferenceStringSequence (abstract|internal)
SuperString::ReferenceStringSequence::~ReferenceStringSequence() {
    // nothing go here
}

//*-- SuperString::ConstASCIISequence (internal)
SuperString::ConstASCIISequence::ConstASCIISequence(const char *chars)
        : _chars(chars),
          _lengthComputed(SuperString::FALSE) {
    // nothing go here
}

SuperString::ConstASCIISequence::~ConstASCIISequence() {
    // nothing go here
}

SuperString::Size SuperString::ConstASCIISequence::length() const /*override*/ {
    if(this->_lengthComputed == SuperString::FALSE) {
        void *ptr;
        const char *pointer = this->_chars;
        while(*pointer != '\0') {
            pointer++;
        }
        // I need to keep this method const
        ptr = ((void *) ((unsigned long) &this->_lengthComputed));
        *((Bool *) ptr) = SuperString::TRUE;
        ptr = ((void *) ((unsigned long) &this->_length));
        *((Size *) ptr) = pointer - this->_chars;
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::ConstASCIISequence::codeUnitAt(
        SuperString::Size index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(*(this->_chars + index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstASCIISequence::substring(SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    void *ptr = ((void *) ((unsigned long) &this->_referencers));
    ((SingleLinkedList<ReferenceStringSequence *> *) ptr)->push(sequence);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::ConstASCIISequence::print(std::ostream &stream) const {
    stream << this->_chars;
}

void SuperString::ConstASCIISequence::print(std::ostream &stream, SuperString::Size startIndex,
                                            SuperString::Size endIndex) const {
    stream.write(this->_chars + startIndex, endIndex - startIndex);
}

SuperString SuperString::ConstASCIISequence::trim() const {
    Size startIndex = 0;
    Size endIndex = this->length();
    char c = *(this->_chars + startIndex);
    while(c != '\0' && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (++startIndex));
    }
    c = *(this->_chars + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (--endIndex - 1));
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::ConstASCIISequence::trimLeft() const {
    Size startIndex = 0;
    char c = *(this->_chars + startIndex);
    while(c != '\0' && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + ++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::ConstASCIISequence::trimRight() const {
    Size endIndex = this->length();
    char c = *(this->_chars + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (--endIndex - 1));
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

//*-- SuperString::SubstringSequence (internal)
SuperString::SubstringSequence::SubstringSequence(const StringSequence *sequence, SuperString::Size startIndex,
                                                  SuperString::Size endIndex) {
    this->_kind = Kind::SUBSTRING;
    this->_container._substring._sequence = sequence;
    this->_container._substring._startIndex = startIndex;
    this->_container._substring._endIndex = endIndex;
    void *ptr = ((void *) ((unsigned long) this->_container._substring._sequence));
    ((StringSequence *) ptr)->refAdd();
}

SuperString::SubstringSequence::~SubstringSequence() {
    if(this->_kind == Kind::SUBSTRING) {
        void *ptr = ((void *) ((unsigned long) this->_container._substring._sequence));
        if(((StringSequence *) ptr)->refRelease() == 0) {
            delete ((StringSequence *) ptr);
        }
    }
}

SuperString::Size SuperString::SubstringSequence::length() const /*override*/ {
    if(this->_kind == Kind::SUBSTRING) {
        return this->_container._substring._endIndex - this->_container._substring._startIndex;
    }
    // TODO:
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::SubstringSequence::codeUnitAt(
        SuperString::Size index) const {
    if(this->_kind == Kind::SUBSTRING) {
        return this->_container._substring._sequence->codeUnitAt(this->_container._substring._startIndex + index);
    }
    // TODO:
    return Result<int, SuperString::Error>(Error::Unimplemented);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::SubstringSequence::substring(SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->_kind == Kind::SUBSTRING) {
        if((this->_container._substring._sequence->length() < (this->_container._substring._startIndex + startIndex)) ||
           (this->_container._substring._sequence->length() < (this->_container._substring._startIndex + endIndex))) {
            return Result<SuperString, Error>(Error::RangeError);
        } else {
            return Result<SuperString, Error>(SuperString(new SubstringSequence(this->_container._substring._sequence,
                                                                                this->_container._substring._startIndex +
                                                                                startIndex,
                                                                                this->_container._substring._startIndex +
                                                                                endIndex)));
        }
    }
    // TODO:
    return Result<SuperString, Error>(Error::Unimplemented);
}

void SuperString::SubstringSequence::print(std::ostream &stream) const {
    this->_container._substring._sequence->print(stream, this->_container._substring._startIndex,
                                                 this->_container._substring._endIndex);
}

void SuperString::SubstringSequence::print(std::ostream &stream, SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    if(this->_kind == Kind::SUBSTRING) {
        this->_container._substring._sequence->print(stream, this->_container._substring._startIndex + startIndex,
                                                     this->_container._substring._startIndex + endIndex);
    }
    // TODO:
}

SuperString SuperString::SubstringSequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::SubstringSequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::SubstringSequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

//*-- SuperString::ConcatenationSequence (internal)
SuperString::ConcatenationSequence::ConcatenationSequence(const StringSequence *leftSequence,
                                                          const StringSequence *rightSequence) {
    this->_kind = Kind::CONCATENATION;
    this->_container._concatenation._left = leftSequence;
    this->_container._concatenation._right = rightSequence;
    void *ptr = ((void *) ((unsigned long) this->_container._concatenation._left));
    ((StringSequence *) ptr)->refAdd();
    ptr = ((void *) ((unsigned long) this->_container._concatenation._right));
    ((StringSequence *) ptr)->refAdd();
}

SuperString::ConcatenationSequence::~ConcatenationSequence() {
    if(this->_kind == Kind::CONCATENATION) {
        void *ptr = ((void *) ((unsigned long) this->_container._concatenation._left));
        if(((StringSequence *) ptr)->refRelease() == 0) {
            delete ((StringSequence *) ptr);
        }
        ptr = ((void *) ((unsigned long) this->_container._concatenation._right));
        if(((StringSequence *) ptr)->refRelease() == 0) {
            delete ((StringSequence *) ptr);
        }
    }
}

SuperString::Size SuperString::ConcatenationSequence::length() const {
    if(this->_kind == Kind::CONCATENATION) {
        return this->_container._concatenation._left->length() + this->_container._concatenation._right->length();
    }
    // TODO:
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::ConcatenationSequence::codeUnitAt(
        SuperString::Size index) const {
    if(this->_kind == Kind::CONCATENATION) {
        if(index < this->_container._concatenation._left->length()) {
            return this->_container._concatenation._left->codeUnitAt(index);
        } else if((index - this->_container._concatenation._left->length()) <
                  this->_container._concatenation._right->length()) {
            return this->_container._concatenation._right->codeUnitAt(
                    index - this->_container._concatenation._left->length());
        } else {
            return Result<int, Error>(Error::RangeError);
        }
    }
    // TODO:
    return Result<int, Error>(Error::Unimplemented);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConcatenationSequence::substring(SuperString::Size startIndex,
                                              SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    void *ptr = ((void *) ((unsigned long) &this->_referencers));
    ((SingleLinkedList<ReferenceStringSequence *> *) ptr)->push(sequence);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::ConcatenationSequence::print(std::ostream &stream) const {
    if(this->_kind == Kind::CONCATENATION) {
        this->_container._concatenation._left->print(stream);
        this->_container._concatenation._right->print(stream);
    }
}

void SuperString::ConcatenationSequence::print(std::ostream &stream, SuperString::Size startIndex,
                                               SuperString::Size endIndex) const {
    if(this->_kind == Kind::CONCATENATION) {
        if(startIndex < this->_container._concatenation._left->length()) {
            if(endIndex < this->_container._concatenation._left->length()) {
                this->_container._concatenation._left->print(stream, startIndex, endIndex);
            } else {
                this->_container._concatenation._left->print(stream, startIndex,
                                                             this->_container._concatenation._left->length());
                this->_container._concatenation._right->print(stream, 0,
                                                              endIndex -
                                                              this->_container._concatenation._left->length());
            }
        } else {
            if((endIndex - this->_container._concatenation._left->length()) <
               this->_container._concatenation._right->length()) {
                this->_container._concatenation._right->print(stream, startIndex -
                                                                      this->_container._concatenation._left->length(),
                                                              endIndex -
                                                              this->_container._concatenation._left->length());
            }
        }
    }
    // TODO:
}

SuperString SuperString::ConcatenationSequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::ConcatenationSequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::ConcatenationSequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

//*-- MultipleSequence (internal)
SuperString::MultipleSequence::MultipleSequence(const StringSequence *sequence, Size time) {
    this->_kind = Kind::MULTIPLE;
    this->_container._multiple._time = time;
    this->_container._multiple._sequence = sequence;
    void *ptr = ((void *) ((unsigned long) this->_container._multiple._sequence));
    ((StringSequence *) ptr)->refAdd();
}

SuperString::MultipleSequence::~MultipleSequence() {
    if(this->_kind == Kind::MULTIPLE) {
        void *ptr = ((void *) ((unsigned long) this->_container._multiple._sequence));
        if(((StringSequence *) ptr)->refRelease() == 0) {
            delete ((StringSequence *) ptr);
        }
    }
}

SuperString::Size SuperString::MultipleSequence::length() const {
    if(this->_kind == Kind::MULTIPLE) {
        return this->_container._multiple._sequence->length() * this->_container._multiple._time;
    }
    // TODO:
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::MultipleSequence::codeUnitAt(SuperString::Size index) const {
    if(this->_kind == Kind::MULTIPLE) {
        Size length = this->length();
        if(index < length) {
            return this->_container._multiple._sequence->codeUnitAt(index % this->_container._multiple._sequence->length());
        }
        return Result<int, Error>(Error::RangeError);
    }
    // TODO:
    return Result<int, Error>(Error::Unimplemented);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::MultipleSequence::substring(SuperString::Size startIndex,
                                         SuperString::Size endIndex) const {
    Size length = this->length();
    if(length < startIndex || length < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    void *ptr = ((void *) ((unsigned long) &this->_referencers));
    ((SingleLinkedList<ReferenceStringSequence *> *) ptr)->push(sequence);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::MultipleSequence::print(std::ostream &stream) const {
    if(this->_kind == Kind::MULTIPLE) {
        for(Size i = 0; i < this->_container._multiple._time; i++) {
            this->_container._multiple._sequence->print(stream);
        }
    }
}

void SuperString::MultipleSequence::print(std::ostream &stream, SuperString::Size startIndex,
                                          SuperString::Size endIndex) const {
    if(this->_kind == Kind::MULTIPLE) {
        Bool printing = FALSE;
        Size unitLength = this->_container._multiple._sequence->length();
        for(Size i = 0; i < this->_container._multiple._time; i++) {
            Size iterationStartIndex = i * unitLength;
            Size iterationEndIndex = (i + 1) * unitLength;
            if(printing == FALSE) {
                if(iterationStartIndex <= startIndex) {
                    if(endIndex < iterationEndIndex) {
                        this->_container._multiple._sequence->print(stream, startIndex - iterationStartIndex,
                                                                    endIndex - iterationStartIndex);
                        break;
                    } else {
                        printing = TRUE;
                        this->_container._multiple._sequence->print(stream, startIndex - iterationStartIndex,
                                                                    unitLength);
                    }
                }
            } else {
                if(endIndex <= iterationEndIndex) {
                    this->_container._multiple._sequence->print(stream, 0, endIndex - iterationStartIndex);
                }
            }
        }
    }
    // TODO:
}

SuperString SuperString::MultipleSequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::MultipleSequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::MultipleSequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

std::ostream &operator<<(std::ostream &stream, const SuperString &string) {
    string.print(stream);
    return stream;
}
