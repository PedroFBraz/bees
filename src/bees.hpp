// Copyright 2023 Pedro Francisco Braz <brazpedrof@outlook.com>.
#include <cstdint>
#include <vector>
#include <array>
#include <stdexcept>
#include <bit>
#include <string_view>
#include <string>
#include <set>
#include <variant>
#include <unordered_map>

namespace bees {

    enum class types : unsigned char {
        f64 = 0x01,
        utf8str = 0x02,
        embedded_doc = 0x03,
        array = 0x04,
        bin_data = 0x05,
        undefined = 0x06, // Deprecated
        object_id = 0x07,
        boolean = 0x08,
        utc_date = 0x09,
        null = 0x0A,
        regex = 0x0B,
        db_ptr = 0x0C, // Deprecated
        js_code = 0x0D,
        symbol = 0x0E, // Deprecated
        code_w_scope = 0x0F, // Deprecated
        i32 = 0x10,
        time_stamp = 0x11,
        i64 = 0x12,
        f128 = 0x13, // Unsupported by Windows, as long doubles are just doubles.
        min_key = 0xFF,
        max_key = 0x7F
    };    
    enum class subtypes : unsigned char {
        generic = 0x00,
        function = 0x01,
        binary = 0x02,
        old_uuid = 0x03,
        uuid = 0x04,
        md5 = 0x05,
        encrypted_bson = 0x06,
        compressed_bson_column = 0x07
    };

    class element;

    class bin_data {
    public:
        bin_data();
        bin_data(int32_t size, std::byte subtype, std::vector<std::byte> data);
        inline bin_data(int32_t size, subtypes subtype, std::vector<std::byte> data);
        int32_t size();
        std::byte& subtype();
        const std::vector<std::byte>& data();
        bin_data& operator=(const bin_data& other);
    private:
        int32_t m_size{};
        std::byte m_subtype{};
        std::vector<std::byte> m_data{};
    };

    /// Deprecated 
    struct undefined {};

    class object_id {
    public:
        object_id();
        /// @brief Constructs an ObjectId.
        /// @param timestamp Big-endian;
        /// @param random_value Little-endian; must not have a value greater than 2^(5*8)-1.
        /// @param counter Big-endian; must not have a greater value than 2^(3*5)-1.
        object_id(uint32_t timestamp, uint64_t random_value, uint32_t counter);
        uint32_t timestamp();
        std::array<unsigned char, 5> random_value();
        std::array<unsigned char, 3> counter();
    private:
        uint32_t m_timestamp{};
        std::array<unsigned char, 5> m_random_value{};
        std::array<unsigned char, 3> m_counter{};
    };

    struct utc_datetime { uint64_t time{}; };
    
    struct null {};

    // TODO: This shouldn't be a class just to hold some data. It would be able to be used in strings of texts.
    // But is this even a good idea? I wouldn't want to lock in the user to use my (probably crappy) regex.
    // Regardless, this is something I'll be worrying about later.  

    class regex {
    public:
        regex(std::u8string_view pattern, std::set<char8_t> options);
    private:
        // m_regex{};
    };

    class db_ptr {
    public:
        /// @brief DBPointer; Deprecated
        /// @param ns Namespace
        /// @param objectid 
        db_ptr(std::u8string_view ns, object_id objectid);
    private:
        std::u8string m_namespace{};
        object_id m_objectid{};
    };

    struct js_code {
        std::u8string m_code{};
    };


    // Deprecated
    struct symbol {
        std::u8string m_symbol{};
    };

    struct timestamp {
        uint64_t m_timestamp{};
    };

    struct min_key{};

    struct max_key{};


    


    class document {
    public:
        document(std::unordered_map<std::u8string_view, element>);
        /// If the key does not exist, a new key, mapping to a null, will be inserted
        /// Throws if inserting an element would cause an integer overflow
        element& operator[](std::u8string_view key);
        /// Throws if the key was not found
        const element& at(std::u8string_view key) const;
    private:
        std::unordered_map<std::u8string, element> m_element_list;
        int m_size{}; 
    };

    /// Deprecated
    class js_code_w_scope {
    public:
        
    private:
        int32_t m_size = 4;
        std::u8string m_code{};
        // std::unordered_map<std::u8string, js_types> m_mappings{};
    };

    class array {
    public:
        /// Passing a negative number will return m_element_list[m_element_list.size() - key]
        /// Throws if key is greater or equal than m_element_list.size() or if -key is greater than m_element_list.size()
        element& operator[](int key);
        
        const element& at(int key) const;
    private:
        std::unordered_map<int, element> m_element_list;
    };
    using possible_types = std::variant<double, std::u8string, document, array, bin_data,
    undefined, object_id, bool, utc_datetime, null,
    regex, db_ptr, js_code, js_code_w_scope, int32_t,
    symbol, timestamp, int64_t, long double, min_key,
    max_key>;
    
    class element {
    public:
        element();
        element(possible_types);
        element& operator=(possible_types);
        element& operator=(const element& other);
        const possible_types& data();
    private:
        possible_types m_data;
        std::byte type;
    };
    document* open(std::u8string_view file_name);
}

