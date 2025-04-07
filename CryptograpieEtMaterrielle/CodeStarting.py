import os
import binascii as bn
import numpy as np
import aeskeyschedule as aeskey  # Ensure this module is available
import matplotlib.pyplot as plt

def CreateFileList_trc(folder):
    """Generate a list of all .trc files in a directory."""
    return [f for f in os.listdir(folder) if f.endswith('.trc')]

def ReadTRACESbin(Folder, FileList, num):
    """Reads a binary trace file and extracts key, plaintext, and ciphertext metadata."""
    FileName = FileList[num]
    Items = FileName.split("_")  # Assuming filename format: AES_key_pti_cto.trc
    key = Items[1]
    keyhex = bn.unhexlify(key)
    K1 = aeskey.key_schedule(keyhex)[0]  
    K10 = aeskey.key_schedule(keyhex)[10]    
    pti = Items[2]
    cto = Items[3].split('.')[0]  # Remove file extension
    
    Key = [int(key[i*2:(i+1)*2], 16) for i in range(16)]
    Pti = [int(pti[i*2:(i+1)*2], 16) for i in range(16)]
    Cto = [int(cto[i*2:(i+1)*2], 16) for i in range(16)]
    
    file = os.path.join(Folder, FileName)
    with open(file, "rb") as f:
        Data = np.fromfile(f, dtype=np.int8)
    
    return Key, Pti, Cto, Data, K1, K10

def analyze_trace(folder):
    """Runs ReadTRACESbin on the first trace file and prints results."""
    FileList = CreateFileList_trc(folder)
    if not FileList:
        print("No .trc files found in the specified folder.")
        return
    
    Key, Pti, Cto, Data, K1, K10 = ReadTRACESbin(folder, FileList, 0)
    
    print("Extracted AES Key:", Key)
    print("Extracted Plaintext:", Pti)
    print("Extracted Ciphertext:", Cto)
    print("First 10 bytes of trace data:", Data[:10])
    
    plt.figure()
    plt.plot(Data)
    plt.title("First Trace Data")
    plt.grid()
    plt.show()

def DoM(folder, num_traces, num_octet):
    """Computes the Difference of Means (DoM) for a given byte index."""
    FileList = CreateFileList_trc(folder)
    if not FileList:
        print("No .trc files found in the specified folder.")
        return None
    
    traces = []
    pt_values = []
    for i in range(num_traces):
        _, Pti, _, Data, _, _ = ReadTRACESbin(folder, FileList, i)
        traces.append(Data)
        pt_values.append(Pti[num_octet])
    
    traces = np.array(traces)
    
    HW_values = [bin(SBOX[pt ^ 0]).count("1") for pt in pt_values]  # HW of SBOX output
    H_high = traces[np.array(HW_values) > 4]
    H_low = traces[np.array(HW_values) < 4]
    
    DoMh = np.abs(np.mean(H_high, axis=0) - np.mean(H_low, axis=0))
    
    plt.figure()
    plt.plot(DoMh, label=f'DoM for byte {num_octet}')
    plt.title(f'DoM Analysis for Byte {num_octet}')
    plt.legend()
    plt.grid()
    plt.show()
    
    return DoMh