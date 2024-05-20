import pandas as pd
import csv
import ipaddress
import glob
import subprocess
import re

def get_eth0_ip_address():
    # Run the `ip a` command and get the output
    result = subprocess.run(['ip', 'a'], stdout=subprocess.PIPE)
    # Decode the output to a string
    output = result.stdout.decode('utf-8')
    # Use a regular expression to find the IP address of the eth0 interface
    match = re.search(r'2: eth0.*?inet (\d+\.\d+\.\d+\.\d+)', output, re.DOTALL)
    # Return the IP address if found
    return match.group(1) if match else None


# Get a list of all CSV files in the current directory
files = glob.glob('*.csv')

subnets_string = "10.220.172.0/24"  # example string
subnets = subnets_string.split(", ")

# Loop over each file
for file in files:
    # Read the CSV file
    data = pd.read_csv(file)

    # Create a new column for subnet and reachable
    data['subnet'] = ''
    data['reachable'] = False

    # Go through each row (ip address) in the data
    for i, row in data.iterrows():
        ip = ipaddress.ip_address(row['host'])  # replace 'ip' with your IP column name
        # Find the subnet that this ip belongs to
        for subnet in subnets:
            if ip in ipaddress.ip_network(subnet):
                data.at[i, 'subnet'] = subnet
                break

        # Grab host and port
        port = str(row['port'])  # replace 'port' with your port column name

        # Run ./portcheck
        result = subprocess.run(['./portcheck', str(ip), port], stdout=subprocess.PIPE)

        # If the application reports back 1, set reachable to True
        if result.stdout.decode('utf-8').strip() == '1':
            data.at[i, 'reachable'] = True
    ip = get_eth0_ip_address()
    # Save the dataframe back to CSV
    data.to_csv(ip+".csv", index=False)
