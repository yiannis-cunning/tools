typedef struct{
	   uint8_t garbage[60];
	   uint32_t offset;
} DOS_header_t;

typedef struct{
	uint16_t signature;
	uint16_t zeros;
	uint16_t machine;
	uint16_t section_count;
	uint32_t time_date_stamp;
	uint32_t symbol_table_offset;
	uint32_t symbol_count;
	uint16_t optional_header_size;
	uint16_t characteristics;
} COFF_header_t;

typedef struct{// Past this assumes PE/PE+ format
    uint16_t magic;
	uint8_t major_linker_version;
	uint8_t minor_linker_version;
	uint32_t code_size;
	uint32_t initialized_data_size;
	uint32_t uninitialized_data_size;
	uint32_t entry_point_address;
	uint32_t base_of_code;

} Shared_header_t; // Also optional header

typedef struct{// Windows specific
	uint32_t image_base;
	uint32_t section_alignment;
	uint32_t file_alignment;
	uint16_t major_operating_system_version;
	uint16_t minor_operating_system_version;
	uint16_t major_image_version;
	uint16_t minor_image_version;
	uint16_t major_subsystem_version;
	uint16_t minor_subsystem_version;
	uint32_t reserved1;
	uint32_t image_size;
	uint32_t header_size;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t dll_characteristics;
	uint32_t stack_reserve_size;
	uint32_t stack_commit_size;
	uint32_t heap_reserve_size;
	uint32_t heap_commit_size;
	uint32_t loader_flags;
	uint32_t number_of_rva_and_sizes;

	uint32_t export_table_rva;
	uint32_t export_table_size;
    uint32_t import_table_rva;
	uint32_t import_table_size;
    uint32_t resource_table_rva;
	uint32_t resource_table_size;
	uint32_t exception_table_rva;
	uint32_t exception_table_size;
    uint32_t certificate_table_rva;
	uint32_t certificate_table_size;
    uint32_t base_relocation_table_rva;
	uint32_t base_relocation_table_size;
    uint32_t debug_data_address_rva;
	uint32_t debug_data_address_size;
    uint32_t reserved2_rva;
    uint32_t reserved2_size;
    uint32_t global_ptr_rva;
    uint32_t global_ptr_size;
    uint32_t tls_table_rva;
	uint32_t tls_table_size;
    uint32_t load_config_table_rva;
	uint32_t load_config_table_size;
    uint32_t bound_input_rva;
	uint32_t bound_input_size;
    uint32_t iat_rva;
	uint32_t iat_size;
    uint32_t delay_import_descriptor_rva;
	uint32_t delay_import_descriptor_size;
    uint32_t clr_runtime_header_rva;
	uint32_t clr_runtime_header_size;
    uint32_t reserved3_rva;
	uint32_t reserved3_size;

} PE32_header_t;

typedef struct{ // Windows specific
	//uint32_t base_of_data;
	uint64_t image_base;
	uint32_t section_alignment;
	uint32_t file_alignment;
	uint16_t major_operating_system_version;
	uint16_t minor_operating_system_version;
	uint16_t major_image_version;
	uint16_t minor_image_version;
	uint16_t major_subsystem_version;
	uint16_t minor_subsystem_version;
	uint32_t reserved1;
	uint32_t image_size;
	uint32_t header_size;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t dll_characteristics;
	uint64_t stack_reserve_size;
	uint64_t stack_commit_size;
	uint64_t heap_reserve_size;
	uint64_t heap_commit_size;
	uint32_t loader_flags;
	uint32_t number_of_rva_and_sizes;

	uint32_t export_table_rva;
	uint32_t export_table_size;
    uint32_t import_table_rva;
	uint32_t import_table_size;
    uint32_t resource_table_rva;
	uint32_t resource_table_size;
	uint32_t exception_table_rva;
	uint32_t exception_table_size;
    uint32_t certificate_table_rva;
	uint32_t certificate_table_size;
    uint32_t base_relocation_table_rva;
	uint32_t base_relocation_table_size;
    uint32_t debug_data_address_rva;
	uint32_t debug_data_address_size;
    uint32_t reserved2_rva;
    uint32_t reserved2_size;
    uint32_t global_ptr_rva;
    uint32_t global_ptr_size;
    uint32_t tls_table_rva;
	uint32_t tls_table_size;
    uint32_t load_config_table_rva;
	uint32_t load_config_table_size;
    uint32_t bound_input_rva;
	uint32_t bound_input_size;
    uint32_t iat_rva;
	uint32_t iat_size;
    uint32_t delay_import_descriptor_rva;
	uint32_t delay_import_descriptor_size;
    uint32_t clr_runtime_header_rva;
	uint32_t clr_runtime_header_size;
    uint32_t reserved3_rva;
	uint32_t reserved3_size;
} PE32_plus_header_t;



typedef struct {
    char name[8];          // Section name (null-padded, 8 characters)
    uint32_t virtual_size; // Virtual size of the section in memory
    uint32_t virtual_address; // Virtual address of the section in memory
    uint32_t size_of_raw_data; // Size of the section in the PE file (on disk)
    uint32_t pointer_to_raw_data; // File pointer to the raw data of the section in the PE file
    uint32_t pointer_to_relocations; // File pointer to the relocation entries for the section
    uint32_t pointer_to_linenumbers; // File pointer to the line number entries for the section (deprecated)
    uint16_t number_of_relocations; // Number of relocation entries for the section
    uint16_t number_of_linenumbers; // Number of line number entries for the section (deprecated)
    uint32_t characteristics; // Flags describing various characteristics of the section
} Section_header_t;


typedef struct{
    // 1) Headers
    DOS_header_t *dos_header;
    COFF_header_t *coff_header;
    Shared_header_t *shared_header;
    uint32_t base_of_data;				   // Only in PE

    PE32_header_t *pe_header;              // Windows specific - PE
    PE32_plus_header_t *pe_plus_header;    // Windows specific - PE+

    // 2) Code headers + content
    Section_header_t *section_headers;
    uint8_t **code_pointers;				// Code alignment in file is given in header

    // 3) string + symbol table
    uint32_t sz_string_table;
    char *string_table;


    int n_sections;
    bool isplus;

} PE_file_t;




typedef struct {
	uint32_t iprt_lookup_table_rva;
	uint32_t time;
	uint32_t forwarder_chain;
	uint32_t name_rva;
	uint32_t iprt_addr_table_rva;

} import_directory_entry_t;


static char *directories_names[] = {
    "Export Table",
    "Import Table",
    "Resource Table",
    "Exception Table",
    "Certificate Table",
    "Base Relocation Table",
    "Debug",
    "RESERVED",
    "Global pointer",
    "Thread Local storage",
    "Load Config table",
    "Bound Import",
    "Import Address Table",
    "Delay Import Descriptor",
    "CLR Runtime Header",
    "RESERVED"
};