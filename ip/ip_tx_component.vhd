COMPONENT ip_tx
    GENERIC (
        -- Input and output bus width in bytes, must be a power of 2
        width : POSITIVE := 8
    );
    PORT (
        -- All ports are assumed to be synchronous with Clk
        Clk : IN STD_LOGIC;
        Rst : IN STD_LOGIC;
        -- Data input bus for the MAC from the UDP module.
        Data_in : IN STD_LOGIC_VECTOR(width * 8 - 1 DOWNTO 0);
        -- Assertion indicates which Data_in bytes are valid. If there are
        -- valid lanes, they will always be consecutive and start with the
        -- first lane. E.g., 0b00111 or 0b11111, but never 0b00110 or 0b01010.
        -- Additionally, the maximum amount of valid data will always be
        -- given, that is, it is only possible for partially valid data on the
        -- last data cycle.
        Data_in_valid : IN STD_LOGIC_VECTOR(width - 1 DOWNTO 0);
        -- Assertion indicates that the first data is available on Data_in.
        Data_in_start : IN STD_LOGIC;
        -- Asserted when the last valid data is available on Data_in.
        Data_in_end : IN STD_LOGIC;
        -- Source IP address. Valid when Data_in_start is asserted.
        Addr_src_in : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        -- Destination IP address. Valid when Data_in_start is asserted.
        Addr_dst_in : IN STD_LOGIC_VECTOR(31 DOWNTO 0);

        -- IPv4 output bus to the MAC. Valid byte lanes depend on
        -- Data_out_valid.
        Data_out : OUT STD_LOGIC_VECTOR(width * 8 - 1 DOWNTO 0);
        -- Assertion indicates which Data_out bytes are valid. If there are
        -- valid lanes, they will always be consecutive and start with the
        -- first lane. E.g., 0b00111 or 0b11111, but never 0b00110 or 0b01010.
        -- Additionally, the maximum amount of valid data will always be
        -- given, that is, it is only possible for partially valid data on the
        -- last data cycle.
        Data_out_valid : OUT STD_LOGIC_VECTOR(width - 1 DOWNTO 0);
        -- Assertion indicates that the first data is available on Data_out.
        Data_out_start : OUT STD_LOGIC;
        -- Asserted when the last data is available on Data_out.
        Data_out_end : OUT STD_LOGIC;
        -- Indicate that there has been an error in the current datagram.
        -- Data_out should be ignored until the next Data_out_start assertion.
        Data_out_err : OUT STD_LOGIC;
        -- IP version and packet length. Valid when Data_out_start is asserted.
        Ver_len_out : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        -- Identification and fragmentation info. Valid when Data_out_start is asserted.
        Id_frag_out : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        -- TTL, protocol, and checksum. Valid when Data_out_start is asserted.
        Ttl_proto_cksm_out : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        -- Source IP address. Valid when Data_out_start is asserted.
        Addr_src_out : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        -- Destination IP address. Valid when Data_out_start is asserted.
        Addr_dst_out : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    );
END COMPONENT;
