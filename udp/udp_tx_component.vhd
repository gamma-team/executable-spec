COMPONENT udp_tx IS
    GENERIC (
        -- Input and output bus width in bytes, must be a power of 2
        width : POSITIVE := 8
    );
    PORT (
        -- All ports are assumed to be synchronous with Clk
        Clk : IN STD_LOGIC;
        Rst : IN STD_LOGIC;
        -- Data input bus for UDP payload data from the application layer.
        -- Byte offsets (all integer types are big endian):
        -- 0: Source IP address (4 bytes)
        -- 4: Destination IP address (4 bytes)
        -- 8: Source port (2 bytes)
        -- 10: Destination port (2 bytes)
        -- 12: Data for UDP datagram data section
        Data_in : IN STD_LOGIC_VECTOR(width * 8 - 1 DOWNTO 0);
        -- Assertion indicates which Data_in bytes are valid.
        Data_in_valid : IN STD_LOGIC_VECTOR(width - 1 DOWNTO 0);
        -- Asserted when the first data is available on Data_in
        Data_in_start : IN STD_LOGIC;
        -- Asserted when the last valid data is available on Data_in.
        Data_in_end : IN STD_LOGIC;

        -- UDP datagram output bus to the IP layer.
        -- Byte offsets (all integer types are big endian):
        -- 0: Source IP address
        -- 4: Destination IP address
        -- 8: UDP datagram
        Data_out : OUT STD_LOGIC_VECTOR(width * 8 - 1 DOWNTO 0);
        -- Assertion indicates which Data_out bytes are valid.
        Data_out_valid : OUT STD_LOGIC_VECTOR(width - 1 DOWNTO 0);
        -- Asserted when the first data is available on Data_out.
        Data_out_start : OUT STD_LOGIC;
        -- Asserted when the last data is available on Data_out.
        Data_out_end : OUT STD_LOGIC;
        -- Indicate that there has been an error in the current datagram.
        -- Data_out should be ignored until the next Data_out_start assertion.
        Data_out_err : OUT STD_LOGIC
    );
END COMPONENT;
