
// For Decoding Unsigned Integer From Given Byte Vector
fn decode_unsigned_int(array : &[u8]) -> u32 {

    let mut inpt : [u8 ; 4] = [0 ; 4];

    
    for (i,&item) in array.iter().rev().enumerate(){
        inpt[3-i] = item;
    }

    u32::from_be_bytes(inpt)
}



// For Decoding Double(Floating Point Nunmber From Given Byte Vector
fn decode_double(array : &[u8]) -> f64 {

    let mut inpt : [u8;8] = [0;8] ;

    for i in 0..array.len()-1{
        inpt[i] = array[i];
    }

    f64::from_be_bytes(inpt)
}



// For Getting Front-End From A Given Byte
fn get_frontend(byte : &u8) -> &str {
    match byte{
        1=>"Zenith Antenna",
        2=>"Starboard Antenna",
        3=>"Port Antenna",
        _=>"Unimplemented"
    }
}


// Returns Sample Indexes 
fn get_sample_indexes(array : &[u8]) -> Vec<u32> {
    
    let mut indexes = Vec::new();

    for i in 0..(array.len()/4){
        indexes.push(decode_unsigned_int(&array[i..i+4]));
    }

    indexes
}


fn main(){
    
    let args : Vec<String>  = std::env::args().collect();

    println!("{}",args.len());
    
    assert!(args.len()==2,"Provide Relative File Name");
    

    let buffer = std::fs::read(&args[1]).unwrap();
    
    
    /* Metadata Read */
    

    println!("\nMetadata Table\n");

    let sat = match buffer[0] {
        0xF7=> "CYG1", 
        0xF9=> "CYG2",  
        0x2B=> "CYG3",  
        0x2C=> "CYG4",  
        0x2F=> "CYG5",  
        0x36=> "CYG6",  
        0x37=> "CYG7",  
        0x49=> "CYG8",  
        _ => "None"
    };

    println!("Satellite Name     : {}",sat);
    
   
    /* DRT0 Packet */


    println!("\nDRT0 Packet\n");

    let packet_name = String::from_utf8_lossy(&buffer[1..5]);

    println!("Packet Name        : {}",packet_name);

    let gps_weeks = decode_unsigned_int(&buffer[5..7]);

    let gps_seconds = decode_unsigned_int(&buffer[7..11]);

    println!("GPS Weeks          : {}",gps_weeks);

    println!("GPS Seconds        : {}",gps_seconds);
    
    let data_format = match buffer[11]{
        0=>"Channel 1,I Only",
        1=>"Channel 1,2 - I Only",
        2=>"Channel 1,2,3 - I Only",
        3=>"Channel 1,2,3,4 - I Only",
        4=>"Channel 1 - I and Q",
        _ => "None"
    };

    println!("Data Format        : {}",data_format);

    let sample_rate = decode_unsigned_int(&buffer[12..16]);

    println!("Sample Rate (Hz)   : {}",sample_rate);

    let front_ends = vec![get_frontend(&buffer[16]),
                        get_frontend(&buffer[21]),
                        get_frontend(&buffer[26]),
                        get_frontend(&buffer[31])];

    let carrier_frequencies = vec![
                          decode_unsigned_int(&buffer[17..21]),
                          decode_unsigned_int(&buffer[22..26]),
                          decode_unsigned_int(&buffer[27..31]),
                          decode_unsigned_int(&buffer[32..36]),
                        ];

    println!("Frontend Channel 1 : {}",front_ends[0]);
    println!("CarrierF Channel 1 : {}",carrier_frequencies[0]);
  
    println!("Frontend Channel 2 : {}",front_ends[1]);
    println!("CarrierF Channel 2 : {}",carrier_frequencies[1]);
    
    println!("Frontend Channel 3 : {}",front_ends[2]);
    println!("CarrierF Channel 3 : {}",carrier_frequencies[2]);
    
    println!("Frontend Channel 4 : {}",front_ends[3]);
    println!("CarrierF Channel 4 : {}",carrier_frequencies[3]);

   
    
    /*  PPS TABLE */
    
    
    println!("\nPPS TABLE\n");

    let last_gps_seconds = decode_double(&buffer[36..45]);

    println!("GPS Seconds Of Last PPS   : {}",last_gps_seconds);

    let sample_indexes = get_sample_indexes(&buffer[45..85]);    
   
    println!("Sample Indexes            : {:?}",sample_indexes);
    
}
