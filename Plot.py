from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np

# Gravitational parameter for Earth in m^3/s^2 (converted from km^3/s^2)
MU_EARTH = 3.986e14  # in meters^3/s^2

# Function to compute orbital elements from state vectors (position and velocity)
def compute_orbital_elements(r, v):
    r = np.array(r)
    v = np.array(v)

    r_mag = np.linalg.norm(r)
    v_mag = np.linalg.norm(v)

    # Specific angular momentum (h = r x v)
    h = np.cross(r, v)
    h_mag = np.linalg.norm(h)

    # Eccentricity vector (e = (v x h)/mu - r/|r|)
    e_vec = (np.cross(v, h) / MU_EARTH) - (r / r_mag)
    e = np.linalg.norm(e_vec)

    # Specific mechanical energy
    epsilon = (v_mag ** 2) / 2 - MU_EARTH / r_mag

    # Semi-major axis (a = -mu / 2*epsilon)
    a = -MU_EARTH / (2 * epsilon)

    # Inclination (i = acos(h_z / |h|))
    i = np.degrees(np.arccos(h[2] / h_mag))

    # Node vector (n = k x h)
    k = np.array([0, 0, 1])
    n = np.cross(k, h)
    n_mag = np.linalg.norm(n)

    # Right Ascension of the Ascending Node (Omega)
    if n_mag != 0:
        Omega = np.degrees(np.arccos(n[0] / n_mag))
        if n[1] < 0:
            Omega = 360 - Omega
    else:
        Omega = 0  # For equatorial orbits

    # Argument of periapsis (omega)
    if n_mag != 0 and e != 0:
        omega = np.degrees(np.arccos(np.dot(n, e_vec) / (n_mag * e)))
        if e_vec[2] < 0:
            omega = 360 - omega
    else:
        omega = 0  # For circular or equatorial orbits

    # True anomaly (nu)
    if e != 0:
        nu = np.degrees(np.arccos(np.dot(e_vec, r) / (e * r_mag)))
        if np.dot(r, v) < 0:
            nu = 360 - nu
    else:
        nu = 0  # For circular orbits

    return {
        "semi_major_axis (a)": a,
        "eccentricity (e)": e,
        "inclination (i)": i,
        "RAAN (Omega)": Omega,
        "argument_of_periapsis (omega)": omega,
        "true_anomaly (nu)": nu
    }

# Function to read data from a file
def read_data(filename):
    data = {}
    with open(filename, 'r') as file:
        lines = file.readlines()
        header = lines[0].strip().split()
        for line in lines[1:]:
            parts = line.strip().split()
            name = ' '.join(parts[:-3])  # Join all parts except the last three as the name
            x, y, z = map(float, parts[-3:])  # The last three are X, Y, Z
            if name not in data:
                data[name] = {'X': [], 'Y': [], 'Z': []}
            data[name]['X'].append(x)
            data[name]['Y'].append(y)
            data[name]['Z'].append(z)
    return data

# Function to plot the orbit and celestial body data
def plot_data(planet_data, satellite_data, orbital_elements):
    # Extract orbital elements from the dictionary
    a = orbital_elements["semi_major_axis (a)"]
    e = orbital_elements["eccentricity (e)"]
    i = np.radians(orbital_elements["inclination (i)"])
    Omega = np.radians(orbital_elements["RAAN (Omega)"])
    omega = np.radians(orbital_elements["argument_of_periapsis (omega)"])

    # Create an array of true anomaly values (0° to 360°)
    true_anomalies = np.linspace(0, 2 * np.pi, 500)

    # Compute the radius for each true anomaly using the orbit equation
    r = a * (1 - e**2) / (1 + e * np.cos(true_anomalies))

    # Compute positions in the orbital plane (x_orbital, y_orbital)
    x_orbital = r * np.cos(true_anomalies)
    y_orbital = r * np.sin(true_anomalies)
    z_orbital = np.zeros_like(x_orbital)  # z in the orbital plane is zero

    # Rotation matrices to rotate from the orbital plane to the inertial frame
    R_RAAN = np.array([[np.cos(Omega), -np.sin(Omega), 0],
                       [np.sin(Omega), np.cos(Omega), 0],
                       [0, 0, 1]])

    R_i = np.array([[1, 0, 0],
                    [0, np.cos(i), -np.sin(i)],
                    [0, np.sin(i), np.cos(i)]])

    R_omega = np.array([[np.cos(omega), -np.sin(omega), 0],
                        [np.sin(omega), np.cos(omega), 0],
                        [0, 0, 1]])

    R_total = R_RAAN @ R_i @ R_omega
    positions_orbital = np.vstack((x_orbital, y_orbital, z_orbital))
    positions_inertial = R_total @ positions_orbital

    x_inertial = positions_inertial[0, :]
    y_inertial = positions_inertial[1, :]
    z_inertial = positions_inertial[2, :]

    # Plot celestial body and satellite data
    fig = plt.figure(figsize=(14, 7))
    
    ax1 = fig.add_subplot(131, projection='3d')
    for planet_name, coords in planet_data.items():
        ax1.plot(coords['X'], coords['Y'], coords['Z'], label=planet_name)
    ax1.set_title('Planet Data (in meters)')
    ax1.set_xlabel('X (m)')
    ax1.set_ylabel('Y (m)')
    ax1.set_zlabel('Z (m)')
    
    ax2 = fig.add_subplot(132, projection='3d')
    for satellite_name, coords in satellite_data.items():
        ax2.plot(coords['X'], coords['Y'], coords['Z'], label=satellite_name, linestyle='--')
    ax2.plot(x_inertial, y_inertial, z_inertial, label='Orbit Path')
    ax2.set_title('Satellite Orbit (in meters)')
    ax2.set_xlabel('X (m)')
    ax2.set_ylabel('Y (m)')
    ax2.set_zlabel('Z (m)')

    ax3 = fig.add_subplot(133, projection='3d')
    for planet_name, coords in planet_data.items():
        ax3.plot(coords['X'], coords['Y'], coords['Z'], label=planet_name)
    for satellite_name, coords in satellite_data.items():
        ax3.plot(coords['X'], coords['Y'], coords['Z'], label=satellite_name, linestyle='--')
    ax3.set_title('Combined Plot (in meters)')
    ax3.set_xlabel('X (m)')
    ax3.set_ylabel('Y (m)')
    ax3.set_zlabel('Z (m)')
    ax3.legend()

    plt.show()
    
def main():
    # Read the data from text files
    planet_data = read_data('/Users/edward/Documents/SatelliteOrbitSimulation/data/planet_data.txt')
    satellite_data = read_data('/Users/edward/Documents/SatelliteOrbitSimulation/data/satellite_data.txt')

    position_vector = [4.21382e+07, 1.61148e+06, 1188.98]  # Example position vector in km
    velocity_vector = [-102.5, 3206.83, 4.72002]  # Example velocity vector in km/s

    orbital_elements = compute_orbital_elements(position_vector, velocity_vector)

    # Plot the data
    plot_data(planet_data, satellite_data, orbital_elements)

if __name__ == '__main__':
    main()
