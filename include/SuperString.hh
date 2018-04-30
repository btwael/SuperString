#ifndef BOUTGLAY_SUPERSTRING_HEADER
#define BOUTGLAY_SUPERSTRING_HEADER

/*-- imports --*/

// std
#include <cstddef>
#include <iostream>

/*-- declarations --*/

/**
 * `SuperString` is an automatically garbage collected string designed
 * with memory efficiency in mind.
 */
class SuperString {
public:
    //*-- Encoding
    /**
     * Supported encoding.
     */
    enum class Encoding {
        ASCII,
        UTF8,
        UTF16BE,
        UTF32
        // TODO: Support LE version of UTF-16
    };

    //*-- Error
    /**
     * Possible errors that SuperString methods can produce.
     */
    enum class Error {
        Unimplemented, // temporary
        Unexpected, // Something that never happen, Unreachable code
        RangeError,
        InvalidByteSequence,
        NotFound
    };

    //*-- Byte
    /**
     * An unsigned 8-bit type, mainly to represent raw data.
     */
    typedef unsigned char Byte;

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

        Result();

        Result(T ok);

        Result(E err);

        Result(const SuperString::Result<T, E> &other) /*copy*/;

        //*- Destructor

        ~Result();

        //*- Getters

        /**
         * Returns the error value.
         */
        E err() const;

        /**
         * Returns true if the result is Ok.
         */
        bool isErr() const;

        /**
         * Returns true if the result is Err.
         */
        bool isOk() const;

        /**
         * Returns the success value.
         */
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
     * Returns true if this string is empty.
     */
    bool isEmpty() const;

    /**
     * Returns true if this string is not empty.
     */
    bool isNotEmpty() const;

    /**
     * Returns the length of this string.
     */
    std::size_t length() const;

    //*- Methods

    /**
     * Returns the 32-bit code unit at the given index.
     */
    SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const;

    /**
     * Compares this to [other].
     */
    int compareTo(const SuperString &other) const;

    /**
     * Returns the position of the first occurrence of [other] in this string,
     * if not found, it returns SuperString::Error::NotFound.
     */
    SuperString::Result<std::size_t, SuperString::Error> indexOf(SuperString other) const;

    /**
     * Returns the position of the last occurrence of [other] in this string,
     * if not found, returns SuperString::Error::NotFound.
     */
    SuperString::Result<std::size_t, SuperString::Error> lastIndexOf(SuperString other) const;

    /**
     * Returns the substring of this sequence that extends
     * from [startIndex], inclusive, to [endIndex], exclusive.
     */
    SuperString::Result<SuperString, SuperString::Error>
    substring(std::size_t startIndex, std::size_t endIndex) const;

    SuperString substr(std::size_t pos, std::size_t len) const;
    SuperString substr(std::size_t pos) const;

    /**
     * Outputs the whole string to the given [stream].
     */
    bool print(std::ostream &stream) const;

    /**
     * Outputs the substring from [startIndex] to [endIndex]
     * to the given [stream].
     */
    bool print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const;

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

    // TODO: delete this two methods
    std::size_t keepingCost() const;

    std::size_t freeingCost() const;

    //*- Operators

    /**
     * Creates a new string by concatenating this string with [other].
     */
    SuperString operator+(const SuperString &other) const;

    /**
     * Creates a new string by concatenating this string with itself a
     * number of [times].
     */
    SuperString operator*(std::size_t times) const;

    /**
     * Assigns [other] to this string.
     */
    SuperString &operator=(const SuperString &other);

    /**
     * Returns `SuperString::TRUE` if this is equal to [other].
     */
    bool operator==(const SuperString &other) const;

    //*- Statics

    /**
     * Creates a string for the given `const char *` [chars] (UTF-8 default as encoding),
     * without copying the data of [chars].
     */
    static SuperString Const(const char *chars, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

    /**
     * Creates a string for the given `const int *` [chars] (UTF-32 default as encoding),
     * without copying the data of [chars].
     */
    static SuperString Const(const int *chars, SuperString::Encoding encoding = SuperString::Encoding::UTF32);

    /**
     * Creates a string for the given `const SuperString::Byte *` [bytes] (UTF-8 default as encoding),
     * without copying the data of [bytes].
     */
    static SuperString
    Const(const SuperString::Byte *bytes, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

    /**
     * Creates a string for the given `const char *` [chars] (UTF-8 default as encoding),
     * by copying the data of [chars].
     */
    static SuperString Copy(const char *chars, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

    /**
     * Creates a string for the given `const int *` [chars] (UTF-32 default as encoding),
     * by copying the data of [chars].
     */
    static SuperString Copy(const int *chars, SuperString::Encoding encoding = SuperString::Encoding::UTF32);

    /**
     * Creates a string for the given `const SuperString::Byte *` [bytes] (UTF-8 default as encoding),
     * by copying the data of [bytes].
     */
    static SuperString
    Copy(const SuperString::Byte *bytes, SuperString::Encoding encoding = SuperString::Encoding::UTF8);

private:
    // forward declaration
    class StringSequence;

    class ReferenceStringSequence;

    class CopyASCIISequence;

    class CopyUTF8Sequence;

    class CopyUTF16BESequence;

    class CopyUTF32Sequence;

    //*-- SuperString
    StringSequence *_sequence;

    //*- Constructors

    SuperString(StringSequence *sequence);

    //*-- SingleLinkedList<E> (internal)
    template<class E>
    class SingleLinkedList {
    public:
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

        //*- Getters

        std::size_t length() const;

        //*- Methods

        void push(E value);

        void remove(E value);

        template<class Ei>
        class Node {
        public:
            Ei _data;
            Node<E> *_next;

            //*- Constructors

            Node(E data);
        };
    };

    //*-- Pair<T, U>
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

    //*-- StringSequence (abstract|internal)
    class StringSequence {
    private:
        std::size_t _refCount;
        SingleLinkedList<ReferenceStringSequence *> _referencers;

    public:
        // Constructors

        StringSequence();

        //*- Destructor

        /**
         * Destructs this sequence.
         */
        virtual ~StringSequence();

        //*- Getters

        /**
         * Returns `SuperString::TRUE` if this string is empty.
         */
        virtual bool isEmpty() const;

        /**
         * Returns `SuperString::TRUE` if this string is not empty.
         */
        virtual bool isNotEmpty() const;

        /**
         * Returns the length of this sequence.
         */
        virtual std::size_t length() const = 0;

        //*- Methods

        /**
         * Returns the 32-bit code unit at the given index.
         */
        virtual SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const = 0;

        SuperString::Result<std::size_t, SuperString::Error> indexOf(SuperString other) const;

        SuperString::Result<std::size_t, SuperString::Error> lastIndexOf(SuperString other) const;

        /**
         * Returns the substring of this sequence that extends
         * from [startIndex], inclusive, to [endIndex], exclusive.
         */
        virtual SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const = 0;

        /**
         * Prints this string to the given [stream].
         */
        virtual bool print(std::ostream &stream) const = 0;

        /**
         * Prints a substring of this string that starts at [startIndex], inclusive,
         * and end at [endIndex], exclusive, to given [stream].
         */
        virtual bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const = 0;

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

        // TODO: comment
        virtual std::size_t keepingCost() const = 0;

        // TODO: comment
        std::size_t freeingCost() const;

        // TODO: comment
        void refAdd() const;

        // TODO: comment
        std::size_t refRelease() const;

        // TODO: comment
        std::size_t refCount() const;

        // TODO: comment
        void addReferencer(SuperString::ReferenceStringSequence *sequence) const;

        // TODO: comment
        void removeReferencer(SuperString::ReferenceStringSequence *sequence) const;

        // TODO: comment
        void reconstructReferencers();

    protected:
        virtual void doDelete() const = 0;

        virtual bool isToBeDeleted() const = 0;

    private:
        bool _substringMatches(std::size_t startIndex, SuperString other) const;

        friend class SuperString;
    };

    //*-- ReferenceStringSequence (abstract|internal)
    class ReferenceStringSequence: public StringSequence {
    public:
        //*- Destructor

        virtual ~ReferenceStringSequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        virtual std::size_t length() const = 0 /*override*/;

        //*- Methods

        virtual SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const = 0 /*override*/;

        virtual SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const = 0 /*override*/;

        virtual bool print(std::ostream &stream) const = 0 /*override*/;

        virtual bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const = 0 /*override*/;

        virtual SuperString trim() const = 0 /*override*/;

        virtual SuperString trimLeft() const = 0 /*override*/;

        virtual SuperString trimRight() const = 0 /*override*/;

        virtual std::size_t keepingCost() const = 0 /*override*/;

        // inherited: std::size_t freeingCost() const;

        // TODO: comment
        virtual std::size_t reconstructionCost(const StringSequence *sequence) const = 0;

        // TODO: comment
        virtual void reconstruct(const StringSequence *sequence) const = 0;

    protected:
        virtual void doDelete() const = 0;

        virtual bool isToBeDeleted() const = 0;
    };

    //*-- ConstASCIISequence (internal)
    class ConstASCIISequence: public StringSequence {
    private:
        enum class Status {
            LengthNotComputed,
            LengthComputed,
            ToBeDestructed
        };

        const Byte *_bytes;
        std::size_t _length;
        Status _status;

    public:
        //*- Constructors

        ConstASCIISequence(const Byte *bytes);

        //*- Destructor

        ~ConstASCIISequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

        friend class CopyASCIISequence;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- CopyASCIISequence (internal)
    class CopyASCIISequence: public StringSequence {
    private:
        Byte *_data;
        std::size_t _length;

    public:
        //*- Constructors

        CopyASCIISequence(const SuperString::Byte *chars);

        CopyASCIISequence(const SuperString::ConstASCIISequence *sequence);

        //*- Destructor

        ~CopyASCIISequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*--ConstUTF8Sequence (internal)
    class ConstUTF8Sequence: public StringSequence {
    private:
        enum class Status {
            LengthNotComputed,
            LengthComputed,
            ToBeDestructed
        };

        const Byte *_bytes;
        std::size_t _length;
        Status _status;

    public:
        //*- Constructors

        ConstUTF8Sequence(const Byte *chars);

        //*- Destructor

        ~ConstUTF8Sequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited:SuperString:: std::size_t freeingCost() const;

        friend class CopyUTF8Sequence;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- CopyUTF8Sequence (internal)
    class CopyUTF8Sequence: public StringSequence {
    private:
        Byte *_data;
        std::size_t _length;
        std::size_t _memoryLength;

    public:
        //*- Constructors

        CopyUTF8Sequence(const SuperString::Byte *chars);

        CopyUTF8Sequence(const SuperString::ConstUTF8Sequence *sequence);

        //*- Destructor

        ~CopyUTF8Sequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- ConstUTF16BESequence (internal)
    class ConstUTF16BESequence: public StringSequence {
    private:
        enum class Status {
            LengthNotComputed,
            LengthComputed,
            ToBeDestructed
        };

        const Byte *_bytes;
        std::size_t _length;
        Status _status;

    public:
        //*- Constructors

        ConstUTF16BESequence(const SuperString::Byte *bytes);

        //*- Destructor

        ~ConstUTF16BESequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited:SuperString:: std::size_t freeingCost() const;

        friend class CopyUTF16BESequence;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- CopyUTF16BESequence (internal)
    class CopyUTF16BESequence: public StringSequence {
    private:
        Byte *_data;
        std::size_t _length;
        std::size_t _memoryLength;

    public:
        //*- Constructors

        CopyUTF16BESequence(const SuperString::Byte *bytes);

        CopyUTF16BESequence(const SuperString::ConstUTF16BESequence *sequence);

        //*- Destructor

        ~CopyUTF16BESequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- ConstUTF32Sequence (internal)
    class ConstUTF32Sequence: public StringSequence {
    private:
        enum class Status {
            LengthNotComputed,
            LengthComputed,
            ToBeDestructed
        };

        const int *_bytes;
        std::size_t _length;
        Status _status;

    public:
        //*- Constructors

        ConstUTF32Sequence(const SuperString::Byte *bytes);

        //*- Destructor

        ~ConstUTF32Sequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

        friend class CopyUTF32Sequence;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- CopyUTF32Sequence (internal)
    class CopyUTF32Sequence: public StringSequence {
    private:
        int *_data;
        std::size_t _length;

    public:
        //*- Constructors

        CopyUTF32Sequence(const SuperString::Byte *chars);

        CopyUTF32Sequence(const SuperString::ConstUTF32Sequence *sequence);

        //*- Destructor

        ~CopyUTF32Sequence();

        //*- Getters

        // inherited: bool isEmpty() const;

        // inherited: bool isNotEmpty() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- SubstringSequence (internal)
    class SubstringSequence: public ReferenceStringSequence {
    private:
        enum class Kind: char {
            SUBSTRING,
            RECONSTRUCTED
        };
        struct SubstringMetaInfo {
            const StringSequence *_sequence;
            std::size_t _startIndex;
            std::size_t _endIndex;
        };
        struct ReconstructedMetaInfo {
            int *_data;
            std::size_t _length;
        };

        Kind _kind;
        union {
            struct SubstringMetaInfo _substring;
            struct ReconstructedMetaInfo _reconstructed;
        } _container;

    public:
        //*- Constructors

        SubstringSequence(const StringSequence *sequence, std::size_t startIndex, std::size_t endIndex);

        //*- Destructor

        ~SubstringSequence();

        //*- Getters

        SuperString::SubstringSequence::Kind kind() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

        std::size_t reconstructionCost(const StringSequence *sequence) const /*override*/;

        void reconstruct(const StringSequence *sequence) const /*override*/;

        friend class StringSequence;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- ConcatenationSequence (internal)
    class ConcatenationSequence: public ReferenceStringSequence {
    private:
        enum class Kind {
            CONCATENATION,
            LEFTRECONSTRUCTED,
            RIGHTRECONSTRUCTED,
            RECONSTRUCTED
        };
        struct ConcatenationMetaInfo {
            const StringSequence *_left;
            const StringSequence *_right;
        };
        struct LeftReconstructedMetaInfo {
            const StringSequence *_right;
            int *_leftData;
            std::size_t _leftLength;
        };
        struct RightReconstructedMetaInfo {
            const StringSequence *_left;
            int *_rightData;
            std::size_t _rightLength;
        };
        struct ReconstructedMetaInfo {
            int *_data;
            std::size_t _length;
        };

        Kind _kind;
        union {
            struct ConcatenationMetaInfo _concatenation;
            struct LeftReconstructedMetaInfo _leftReconstructed;
            struct RightReconstructedMetaInfo _rightReconstructed;
            struct ReconstructedMetaInfo _reconstructed;
        } _container;

    public:
        //*- Constructors

        ConcatenationSequence(const StringSequence *leftSequence, const StringSequence *rightSequence);

        //*- Destructor

        ~ConcatenationSequence();

        //*- Getters

        SuperString::ConcatenationSequence::Kind kind() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

        std::size_t reconstructionCost(const StringSequence *sequence) const /*override*/;

        void reconstruct(const StringSequence *sequence) const /*override*/;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    //*-- MultipleSequence (internal)
    class MultipleSequence: public ReferenceStringSequence {
    private:
        struct MultipleMetaInfo {
            std::size_t _time;
            const StringSequence *_sequence;
        };
        struct ReconstructedMetaInfo {
            std::size_t _time;
            int *_data;
            std::size_t _dataLength;
        };
        enum class Kind {
            MULTIPLE,
            RECONSTRUCTED
        };

        Kind _kind;
        union {
            struct MultipleMetaInfo _multiple;
            struct ReconstructedMetaInfo _reconstructed;
        } _container;

    public:
        //*- Constructors

        MultipleSequence(const StringSequence *sequence, std::size_t time);

        //*- Destructor

        ~MultipleSequence();

        //*- Getters

        SuperString::MultipleSequence::Kind kind() const;

        std::size_t length() const /*override*/;

        //*- Methods

        SuperString::Result<int, SuperString::Error> codeUnitAt(std::size_t index) const /*override*/;

        SuperString::Result<SuperString, SuperString::Error>
        substring(std::size_t startIndex, std::size_t endIndex) const /*override*/;

        bool print(std::ostream &stream) const /*override*/;

        bool
        print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const /*override*/;

        SuperString trim() const /*override*/;

        SuperString trimLeft() const /*override*/;

        SuperString trimRight() const /*override*/;

        std::size_t keepingCost() const /*override*/;

        // inherited: std::size_t freeingCost() const;

        std::size_t reconstructionCost(const StringSequence *sequence) const /*override*/;

        void reconstruct(const StringSequence *sequence) const /*override*/;

    protected:
        void doDelete() const;

        bool isToBeDeleted() const;
    };

    inline static bool isWhiteSpace(int codeUnit);

    //
    class ASCII {
    public:
        static std::size_t length(const SuperString::Byte *bytes);

        static int codeUnitAt(const SuperString::Byte *bytes, std::size_t index);

        static void print(std::ostream &stream, const SuperString::Byte *bytes);

        static void print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                          std::size_t endIndex);

        static SuperString::Pair<std::size_t, std::size_t>
        trim(const SuperString::Byte *bytes, std::size_t length);

        static std::size_t trimLeft(const SuperString::Byte *bytes);

        static std::size_t trimRight(const SuperString::Byte *bytes, std::size_t length);
    };

    class UTF8 {
    public:
        static std::size_t length(const SuperString::Byte *bytes);

        static SuperString::Pair<std::size_t, std::size_t>
        lengthAndMemoryLength(const SuperString::Byte *bytes);

        static SuperString::Result<int, SuperString::Error>
        codeUnitAt(const SuperString::Byte *bytes, std::size_t index);

        static void print(std::ostream &stream, const SuperString::Byte *bytes);

        static void print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                          std::size_t endIndex);

        static SuperString::Result<SuperString::Pair<std::size_t, std::size_t>, SuperString::Error>
        rangeIndexes(const SuperString::Byte *bytes, std::size_t startIndex, std::size_t endIndex);

        static SuperString::Pair<SuperString::Byte *, std::size_t> codeUnitToChar(int c);

        // TODO: add customized trims methods
    };

    class UTF16BE {
    public:
        static std::size_t length(const SuperString::Byte *bytes);

        static SuperString::Pair<std::size_t, std::size_t>
        lengthAndMemoryLength(const SuperString::Byte *bytes);

        static SuperString::Result<int, SuperString::Error>
        codeUnitAt(const SuperString::Byte *bytes, std::size_t index);

        static void print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t length);

        static void print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                          std::size_t endIndex);

        // TODO: add customized trims methods
    };

    class UTF32 {
    public:
        static std::size_t length(const SuperString::Byte *bytes);

        static SuperString::Pair<std::size_t, std::size_t>
        lengthAndMemoryLength(const SuperString::Byte *bytes);

        static int codeUnitAt(const SuperString::Byte *bytes, std::size_t index);

        static void print(std::ostream &stream, const SuperString::Byte *bytes);

        static void print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                          std::size_t endIndex);

        static SuperString::Pair<std::size_t, std::size_t>
        trim(const SuperString::Byte *bytes, std::size_t length);

        static std::size_t trimLeft(const SuperString::Byte *bytes);

        static std::size_t trimRight(const SuperString::Byte *bytes, std::size_t length);
    };
};

// External Operators

std::ostream &operator<<(std::ostream &stream, const SuperString &string);

/*-- definitions --*/

//*-- SuperString::Result<T, E>
template<class T, class E>
SuperString::Result<T, E>::Result()
        : _ok(NULL),
          _err(NULL) {
    // nothing go here
}

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
bool SuperString::Result<T, E>::isErr() const {
    return this->_err != NULL;
}

template<class T, class E>
bool SuperString::Result<T, E>::isOk() const {
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
        delete tmp;
    }
    this->_head = NULL;
    this->_tail = NULL;
}

template<class E>
std::size_t SuperString::SingleLinkedList<E>::length() const {
    std::size_t length = 0;
    Node<E> *node = this->_head;
    while(node != NULL) {
        length++;
        node = node->_next;
    }
    return length;
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

template<class E>
void SuperString::SingleLinkedList<E>::remove(E value) {
    Node<E> *prev = NULL;
    Node<E> *node = this->_head;
    while(node != NULL) {
        if(node->_data == value) {
            break;
        }
        prev = node;
        node = node->_next;
    }
    if(node != NULL) {
        if(prev == NULL) {
            this->_head = node->_next;
        } else {
            prev->_next = node->_next;
        }
        if(node->_next == NULL) {
            this->_tail = prev;
        }
        delete node;
    }
}

//*-- SuperString::SingleLinkedList<E>::Node<Ei> (internal)
template<class E>
template<class Ei>
SuperString::SingleLinkedList<E>::Node<Ei>::Node(E data)
        : _data(data),
          _next(NULL) {
    // nothing go here
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

//*-- SuperString (statics)
bool SuperString::isWhiteSpace(int codeUnit) {
    if(codeUnit <= 32) {
        return (
                (codeUnit == 32) || // Space.
                ((codeUnit <= 13) && (codeUnit >= 9)) // CR, LF, TAB, etc.
        );
    }
    return (codeUnit == 0x85) || (codeUnit == 0xA0); // NEL, NBSP.
}


#endif // BOUTGLAY_SUPERSTRING_HEADER
