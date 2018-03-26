#ifndef BOUTGLAY_SUPERSTRING_HEADER
#define BOUTGLAY_SUPERSTRING_HEADER

/*-- imports --*/

// std
#include <iostream> // The only thing we need, and just for printing

// in case NULL is not defined
#ifndef NULL
#define NULL (0L)
#endif // NULL

/*-- declarations --*/

/**
 * `SuperString` is an automatically garbage collected string designed
 * with memory optimisation in mind.
 */
class SuperString {
public:
    //*-- Encoding
    /**
     * Supported encoding.
     */
    enum class Encoding {
        ASCII,
        UTF8
    };

    //*-- Error
    /**
     * Possible errors that SuperString methods can produce.
     */
    enum class Error {
        Unimplemented, // temporary
        Unexpected, // Something that never happen, Unreachable code
        RangeError,
        InvalidByteSequence
    };

    //*-- Byte
    // TODO: comment this.
    typedef unsigned char Byte;

    //*-- Size
    /**
     * `Size` is an unsigned long type used to represent indexes and offsets..
     */
    typedef unsigned long Size;

    //*-- Bool
    /**
     * The famous boolean type with its two values `FALSE` and `TRUE`.
     */
    typedef unsigned char Bool;
    static const Bool FALSE = ((Bool) 0);
    static const Bool TRUE = ((Bool) 1);

    //*-- Result<T, E>
    /**
     * `Result<T, E>` is the type used for returning and propagating errors.
     */
    template<class T, class E>
    class Result {
    private:
        char *_ok;
        char *_err;

    public:
        //*- Constructors

        Result(T ok);

        Result(E err);

        Result(const SuperString::Result<T, E> &other) /*copy*/;

        //*- Destructor

        ~Result();

        //*- Getters

        E err() const;

        /**
         * Returns `TRUE` if the result is Ok.
         */
        SuperString::Bool isErr() const;

        /**
         * Returns `TRUE` if the result is Err.
         */
        SuperString::Bool isOk() const;

        T ok() const;

        //*- Setters

        /**
         * Sets this to Err with given [err] value.
         */
        void err(E err);

        /**
         * Sets this to Ok with given [ok] value.
         */
        void ok(T ok);

        //*- Operators

        SuperString::Result<T, E> &operator=(const SuperString::Result<T, E> &other);
    };

    //*-- Pair<T, U>
    // TODO: comments this
    template<class T, class U>
    class Pair {
    private:
        T _0;
        U _1;

    public:
        //*- Constructor
        Pair();

        Pair(T $0, U $1);

        //*- Getters

        T first() const;

        U second() const;

        //*- Setters

        void first(T $0);

        void second(U $1);
    };

    //*-- SuperString
public:
    //*- Constructors

    /**
     * Constructs a new empty string.
     */
    SuperString();

    /**
     * Constructs a new string from [other].
     */
    SuperString(const SuperString &other) /*copy*/;

    //*- Destructor

    /**
     * Destructs this string.
     */
    ~SuperString();

    //*- Getters

    /**
     * Returns the length of this string.
     */
    SuperString::Size length() const;

    //*- Methods

    /**
     * Returns the 32-bit UTF-16 code unit at the given index.
     */
    SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const;

    /**
     * Compares this to [other].
     */
    int compareTo(const SuperString &other) const;

    /**
     * Returns the substring of this sequence that extends
     * from [startIndex], inclusive, to [endIndex], exclusive.
     */
    SuperString::Result<SuperString, SuperString::Error>
    substring(SuperString::Size startIndex, SuperString::Size endIndex) const;

    /**
     * Outputs the whole string to the given [stream].
     */
    void print(std::ostream &stream) const;

    /**
     * Outputs the substring from [startIndex] to [endIndex]
     * to the given [stream].
     */
    void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const;

    /**
     * Returns the string without any leading and trailing whitespace.
     */
    SuperString trim() const;

    /**
     * Returns the string without any leading whitespace.
     */
    SuperString trimLeft() const;

    /**
     * Returns the string without any trailing whitespace.
     */
    SuperString trimRight() const;

    //*- Operators

    /**
     * Creates a new string by concatenating this string with [other].
     */
    SuperString operator+(const SuperString &other) const;

    /**
     * Creates a new string by concatenating this string with itself a
     * number of [times].
     */
    SuperString operator*(Size times) const;

    /**
     * Assigns [other] to this string.
     */
    SuperString &operator=(const SuperString &other);

    /**
     * Returns `TRUE` if this is equal to [other].
     */
    SuperString::Bool operator==(const SuperString &other) const;

    //*- Statics

    /**
     * Creates a string for the given const (char *) [chars].
     */
    static SuperString Const(const char *chars, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

    // TODO: comment
    static SuperString
    Const(const SuperString::Byte *bytes, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

    // TODO: comment
    static SuperString Copy(const char *chars, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

    // TODO: comment
    static SuperString
    Copy(const SuperString::Byte *bytes, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

private:
    // forward declaration
    class StringSequence;

    class ReferenceStringSequence;

    class CopyASCIISequence;

    class CopyUTF8Sequence;

    //*-- SuperString
    StringSequence *_sequence;

    //*- Constructors

    SuperString(StringSequence *sequence);

    //*-- SingleLinkedList<E> (internal)
    template<class E>
    class SingleLinkedList {
    private:
        // forward declaration
        template<class Ei>
        class Node;

        Node<E> *_head;
        Node<E> *_tail;
    public:
        //*- Constructors

        SingleLinkedList();

        //*- Destructor

        ~SingleLinkedList();

        //*- Methods

        void push(E value);

    private:
        template<class Ei>
        class Node {
        public:
            Ei _data;
            Node<E> *_next;

            //*- Constructors

            Node(E data);
        };
    };

    //*-- StringSequence (abstract|internal)
    class StringSequence {
    private:
        SuperString::Size _refCount;
        SingleLinkedList<ReferenceStringSequence *> _referencers;

    public:
        //*- Destructor

        /**
         * Destructs this sequence.
         */
        virtual ~StringSequence();

        //*- Getters

        /**
         * Returns if this string is empty.
         */
        virtual SuperString::Bool isEmpty() const;

        /**
         * Returns if this string is not empty.
         */
        virtual SuperString::Bool isNotEmpty() const;

        /**
         * Returns the length of this sequence.
         */
        virtual SuperString::Size length() const = 0;

        //*- Methods

        /**
         * Returns the 32-bit UTF-16 code unit at the given index.
         */
        virtual SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const = 0;

        /**
         * Returns the substring of this sequence that extends
         * from [startIndex], inclusive, to [endIndex], exclusive.
         */
        virtual SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const = 0;

        virtual void print(std::ostream &stream) const = 0;

        virtual void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const = 0;

        /**
         * Returns the sequence without any leading and trailing whitespace.
         */
        virtual SuperString trim() const = 0;

        /**
         * Returns the string without any leading whitespace.
         */
        virtual SuperString trimLeft() const = 0;

        /**
         * Returns the string without any trailing whitespace.
         */
        virtual SuperString trimRight() const = 0;

    private:
        //*- Methods

        void refAdd();

        SuperString::Size refRelease();

        friend class SuperString;
    };

    //*-- ReferenceStringSequence (abstract|internal)
    class ReferenceStringSequence: public StringSequence {
    public:
        //*- Destructor

        virtual ~ReferenceStringSequence();

        //*- Getters

        // inherited: SuperString::Bool isEmpty() const;

        // inherited: SuperString::Bool isNotEmpty() const;

        virtual SuperString::Size length() const = 0 /*override*/;

        //*- Methods

        virtual SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const = 0 /*override*/;

        virtual SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const = 0 /*override*/;

        virtual void print(std::ostream &stream) const = 0 /*override*/;

        virtual void
        print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const = 0 /*override*/;

        virtual SuperString trim() const = 0 /*override*/;

        virtual SuperString trimLeft() const = 0 /*override*/;

        virtual SuperString trimRight() const = 0 /*override*/;
    };

    //*-- ConstASCIISequence (internal)
    class ConstASCIISequence: public StringSequence {
    private:
        const char *_chars;
        SuperString::Size _length;
        SuperString::Bool _lengthComputed;

    public:
        //*- Constructors

        ConstASCIISequence(const char *chars);

        //*- Destructor

        ~ConstASCIISequence();

        //*- Getters

        // inherited: SuperString::Bool isEmpty() const;

        // inherited: SuperString::Bool isNotEmpty() const;

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        friend class CopyASCIISequence;
    };

    //*-- CopyASCIISequence (internal)
    class CopyASCIISequence: public StringSequence {
    private:
        char *_chars;
        SuperString::Size _length;

    public:
        //*- Constructors

        CopyASCIISequence(const char *chars);

        CopyASCIISequence(const SuperString::ConstASCIISequence *sequence);

        //*- Destructor

        ~CopyASCIISequence();

        //*- Getters

        // inherited: SuperString::Bool isEmpty() const;

        // inherited: SuperString::Bool isNotEmpty() const;

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;
    };

    //*--ConstUTF8Sequence (internal)
    class ConstUTF8Sequence: public StringSequence {
    private:
        const char *_chars;
        SuperString::Size _length;
        SuperString::Bool _lengthComputed;

    public:
        //*- Constructors

        ConstUTF8Sequence(const char *chars);

        //*- Destructor

        ~ConstUTF8Sequence();

        //*- Getters

        // inherited: SuperString::Bool isEmpty() const;

        // inherited: SuperString::Bool isNotEmpty() const;

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        friend class CopyUTF8Sequence;
    };

    //*-- CopyUTF8Sequence (internal)
    class CopyUTF8Sequence: public StringSequence {
    private:
        char *_chars;
        SuperString::Size _length;
        SuperString::Bool _lengthComputed;

    public:
        //*- Constructors

        CopyUTF8Sequence(const char *chars);

        CopyUTF8Sequence(const SuperString::ConstUTF8Sequence *sequence);

        //*- Destructor

        ~CopyUTF8Sequence();

        //*- Getters

        // inherited: SuperString::Bool isEmpty() const;

        // inherited: SuperString::Bool isNotEmpty() const;

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;
    };

    static Result<SuperString::Pair<Size, Size>, SuperString::Error>
    _UTF8_offsetOfRange(const char *chars, Size startIndex, Size endIndex);

    // TODO: ConstUTF16Sequence

    //*-- SubstringSequence (internal)
    class SubstringSequence: public ReferenceStringSequence {
    private:
        enum class Kind {
            SUBSTRING,
            CONTENTED
        };
        Kind _kind;
        union {
            struct {
                const StringSequence *_sequence;
                Size _startIndex;
                Size _endIndex;
            } _substring;
            struct {

            } _contented;
        } _container;

    public:
        //*- Constructors

        SubstringSequence(const StringSequence *sequence, SuperString::Size startIndex, SuperString::Size endIndex);

        //*- Destructor

        ~SubstringSequence();

        //*- Getters

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;
    };

    //*-- ConcatenationSequence (internal)
    class ConcatenationSequence: public ReferenceStringSequence {
    private:
        enum class Kind {
            CONCATENATION,
            CONTENTED
        };
        Kind _kind;
        union {
            struct {
                const StringSequence *_left;
                const StringSequence *_right;
            } _concatenation;
            struct {

            } _contented;
        } _container;

    public:
        //*- Constructors

        ConcatenationSequence(const StringSequence *leftSequence, const StringSequence *rightSequence);

        //*- Destructor

        ~ConcatenationSequence();

        //*- Getters

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;
    };

    //*-- MultipleSequence (internal)
    class MultipleSequence: public ReferenceStringSequence {
    private:
        enum class Kind {
            MULTIPLE,
            CONTENTED
        };
        Kind _kind;
        union {
            struct {
                Size _time;
                const StringSequence *_sequence;
            } _multiple;
            struct {

            } _contented;
        } _container;

    public:
        //*- Constructors

        MultipleSequence(const StringSequence *sequence, Size time);

        //*- Destructor

        ~MultipleSequence();

        //*- Getters

        SuperString::Size length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(SuperString::Size index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        void print(std::ostream &stream) const /*override*/;

        void print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;
    };

    inline static SuperString::Bool isWhiteSpace(int codeUnit);
};

// External Operators

std::ostream &operator<<(std::ostream &stream, const SuperString &string);

/*-- definitions --*/

//*-- SuperString::Result<T, E>
template<class T, class E>
SuperString::Result<T, E>::Result(T ok)
        : _ok(NULL),
          _err(NULL) {
    this->_ok = new char[sizeof(T)];
    new(this->_ok) T(ok);
}

template<class T, class E>
SuperString::Result<T, E>::Result(E err)
        : _ok(NULL),
          _err(NULL) {
    this->_err = new char[sizeof(E)];
    new(this->_err) E(err);
}

template<class T, class E>
SuperString::Result<T, E>::Result(const SuperString::Result<T, E> &other)
        : _ok(NULL),
          _err(NULL) /*copy*/ {
    if(other._ok != NULL) {
        this->_ok = new char[sizeof(T)];
        new(this->_ok) T(*((T *) other._ok));
    } else {
        this->_err = new char[sizeof(E)];
        new(this->_err) E(*((E *) other._err));
    }
}

template<class T, class E>
SuperString::Result<T, E>::~Result() {
    if(this->_ok != NULL) {
        ((T *) (this->_ok))->~T();
        delete this->_ok;
        this->_ok = NULL;
    }
    if(this->_err != NULL) {
        ((E *) (this->_err))->~E();
        delete this->_err;
        this->_err = NULL;
    }
}

template<class T, class E>
E SuperString::Result<T, E>::err() const {
    return *((E *) this->_err);
}

template<class T, class E>
SuperString::Bool SuperString::Result<T, E>::isErr() const {
    return this->_err != NULL;
}

template<class T, class E>
SuperString::Bool SuperString::Result<T, E>::isOk() const {
    return this->_ok != NULL;
}

template<class T, class E>
T SuperString::Result<T, E>::ok() const {
    return *((T *) this->_ok);
}

template<class T, class E>
void SuperString::Result<T, E>::err(E err) {
    this->~Result();
    this->_err = new char[sizeof(E)];
    new(this->_err) E(err);
}

template<class T, class E>
void SuperString::Result<T, E>::ok(T ok) {
    this->~Result();
    this->_ok = new char[sizeof(T)];
    new(this->_ok) T(ok);
}

template<class T, class E>
SuperString::Result<T, E> &SuperString::Result<T, E>::operator=(const SuperString::Result<T, E> &other) {
    if(this != &other) {
        if(other.isOk()) {
            this->ok(other.ok());
        } else {
            this->err(other.err());
        }
    }
    return *this;
}

//*-- SuperString::Pair<T, U>
template<class T, class U>
SuperString::Pair<T, U>::Pair() {
    // nothing go here
}

template<class T, class U>
SuperString::Pair<T, U>::Pair(T $0, U $1)
        : _0($0),
          _1($1) {
    // nothing go here
}

template<class T, class U>
T SuperString::Pair<T, U>::first() const {
    return this->_0;
}

template<class T, class U>
U SuperString::Pair<T, U>::second() const {
    return this->_1;
}

template<class T, class U>
void SuperString::Pair<T, U>::first(T $0) {
    this->_0 = $0;
}

template<class T, class U>
void SuperString::Pair<T, U>::second(U $1) {
    this->_1 = $1;
}

//*-- SuperString::SingleLinkedList<E> (internal)
template<class E>
SuperString::SingleLinkedList<E>::SingleLinkedList()
        : _head(NULL),
          _tail(NULL) {
    // nothing go here
}

template<class E>
SuperString::SingleLinkedList<E>::~SingleLinkedList() {
    Node<E> *node = this->_head;
    while(node != NULL) {
        Node<E> *tmp = node;
        node = node->_next;
        delete node;
    }
    this->_head = NULL;
    this->_tail = NULL;
}

template<class E>
void SuperString::SingleLinkedList<E>::push(E value) {
    Node<E> *node = new Node<E>(value);
    if(this->_tail == NULL) {
        this->_head = node;
        this->_tail = node;
    } else {
        this->_tail->_next = node;
        this->_tail = node;
    }
}

//*-- SuperString::SingleLinkedList<E>::Node<Ei> (internal)
template<class E>
template<class Ei>
SuperString::SingleLinkedList<E>::Node<Ei>::Node(E data)
        : _data(data) {
    // nothing go here
}

//*-- SuperString (statics)
SuperString::Bool SuperString::isWhiteSpace(int codeUnit) {
    if(codeUnit <= 32) {
        return (
                (codeUnit == 32) || // Space.
                ((codeUnit <= 13) && (codeUnit >= 9)) // CR, LF, TAB, etc.
        );
    }
    return (codeUnit == 0x85) || (codeUnit == 0xA0); // NEL, NBSP.
}


#endif // BOUTGLAY_SUPERSTRING_HEADER
