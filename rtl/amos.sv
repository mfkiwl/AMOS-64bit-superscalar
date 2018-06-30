module amos (
    input  logic            clk_i,
    input  logic            rst_ni,
    // asynchronous interrupts
    input  logic [1:0]      async_irq_i,
    input  logic            async_debug_irq_i,
    // read channel
    output logic [AW-1:0]   ar_addr,
    output logic [2:0]      ar_prot,
    output logic [3:0]      ar_region,
    output logic [7:0]      ar_len,
    output logic [2:0]      ar_size,
    output logic [1:0]      ar_burst,
    output logic            ar_lock,
    output logic [3:0]      ar_cache,
    output logic [3:0]      ar_qos,
    output logic [IW-1:0]   ar_id,
    output logic [UW-1:0]   ar_user,
    output logic [3:0]      ar_snoop,
    output logic [1:0]      ar_domain,
    output logic [1:0]      ar_bar,
    input  logic            ar_ready,
    output logic            ar_valid,
    // read data channel
    input  logic [DW-1:0]   r_data,
    input  logic [3:0]      r_resp,
    input  logic            r_last,
    input  logic [IW-1:0]   r_id,
    input  logic [UW-1:0]   r_user,
    output logic            r_ack,
    output logic            r_ready,
    input  logic            r_valid,
    // write channel
    output logic [AW-1:0]   aw_addr,
    output logic [2:0]      aw_prot,
    output logic [3:0]      aw_region,
    output logic [7:0]      aw_len,
    output logic [2:0]      aw_size,
    output logic [1:0]      aw_burst,
    output logic            aw_lock,
    output logic [3:0]      aw_cache,
    output logic [3:0]      aw_qos,
    output logic [IW-1:0]   aw_id,
    output logic [UW-1:0]   aw_user,
    output logic [2:0]      aw_atop,
    output logic [2:0]      aw_snoop,
    output logic [1:0]      aw_domain,
    output logic [1:0]      aw_bar,
    output logic            aw_unique,
    input  logic            aw_ready,
    output logic            aw_valid,
    // write data channel
    output logic [DW-1:0]   w_data,
    output logic [DW/8-1:0] w_strb,
    output logic [UW-1:0]   w_user,
    output logic            w_last,
    output logic            w_ack,
    input  logic            w_ready,
    output logic            w_valid,
    // write response channel
    input  logic [1:0]      b_resp,
    input  logic [IW-1:0]   b_id,
    input  logic [UW-1:0]   b_user,
    output logic            b_ready,
    input  logic            b_valid,
    // snoop address channel
    input  logic [AC-1:0]   ac_addr,
    input  logic [3:0]      ac_snoop,
    input  logic [2:0]      ac_prot,
    output logic            ac_ready,
    input  logic            ac_valid,
    // snoop data channel
    output logic [CD-1:0]   cd_data,
    output logic            cd_last,
    input  logic            cd_ready,
    output logic            cd_valid,
    // snoop response channel
    output logic [4:0]      cr_resp,
    output logic            cr_valid,
    input  logic            cr_ready
);






endmodule