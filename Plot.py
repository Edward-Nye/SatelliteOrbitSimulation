from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np

def read_data(filename):
    """
    Reads the data from a text file.
    
    Args:
        filename (str): The name of the file to read from.
    
    Returns:
        np.ndarray: An array of shape (n, 3) where n is the number of rows.
    """
    data = np.loadtxt(filename, skiprows=1)  # Skip header row
    return data

def plot_data(planet_data, satellite_data):
    """
    Plots planet and satellite data.
    
    Args:
        planet_data (np.ndarray): The data for planets.
        satellite_data (np.ndarray): The data for satellites.

    """

    u = np.linspace(0, 2 * np.pi, 15)
    v = np.linspace(0, np.pi, 15)

    earthX = (12.742E6 * np.outer(np.cos(u), np.sin(v))) + planet_data[-1, 0]
    earthY = (12.742E6 * np.outer(np.sin(u), np.sin(v))) + planet_data[-1, 1]
    earthZ = (12.742E6 * np.outer(np.ones(np.size(u)), np.cos(v))) + planet_data[-1, 2]

    moonX = (3.475E6 * np.outer(np.cos(u), np.sin(v))) + satellite_data[-1, 0]
    moonY = (3.475E6 * np.outer(np.sin(u), np.sin(v))) + satellite_data[-1, 1]
    moonZ = (3.475E6 * np.outer(np.ones(np.size(u)), np.cos(v))) + satellite_data[-1, 2]

    SOIearthX = (0.929E9 * np.outer(np.cos(u), np.sin(v))) + planet_data[-1, 0]
    SOIearthY = (0.929E9 * np.outer(np.sin(u), np.sin(v))) + planet_data[-1, 1]
    SOIearthZ = (0.929E9 * np.outer(np.ones(np.size(u)), np.cos(v))) + planet_data[-1, 2]

    SOImoonX = (0.0643E9 * np.outer(np.cos(u), np.sin(v))) + satellite_data[-1, 0]
    SOImoonY = (0.0643E9 * np.outer(np.sin(u), np.sin(v))) + satellite_data[-1, 1]
    SOImoonZ = (0.0643E9 * np.outer(np.ones(np.size(u)), np.cos(v))) + satellite_data[-1, 2]
    
    fig = plt.figure(figsize=(14, 7))
    
    # Plot planet data
    ax1 = fig.add_subplot(131, projection='3d')
    ax1.plot(planet_data[:, 0], planet_data[:, 1], planet_data[:, 2], c='b')
    ax1.set_title('Planet Data')
    ax1.set_xlabel('X')
    ax1.set_ylabel('Y')
    ax1.set_zlabel('Z')
    
    # Plot satellite data
    ax2 = fig.add_subplot(132, projection='3d')
    ax2.plot(satellite_data[:, 0], satellite_data[:, 1], satellite_data[:, 2], c='r')
    ax2.set_title('Satellite Data')
    ax2.set_xlabel('X')
    ax2.set_ylabel('Y')
    ax2.set_zlabel('Z')

    ax3 = fig.add_subplot(133, projection='3d')
    ax3.plot(planet_data[:, 0], planet_data[:, 1], planet_data[:, 2], c='b', label='Planets')
    ax3.plot(satellite_data[:, 0], satellite_data[:, 1], satellite_data[:, 2], c='r', label='Satellites')
    ax3.plot_wireframe(earthX, earthY, earthZ, linewidth=0.5)
    ax3.plot_wireframe(moonX, moonY, moonZ, linewidth=0.5)
    #ax3.plot_wireframe(SOIearthX, SOIearthY, SOIearthZ, linewidth=0.1)
    #ax3.plot_wireframe(SOImoonX, SOImoonY, SOImoonZ, linewidth=0.1)
    ax3.set_title('Planet and Satellite Data')
    ax3.set_xlabel('X')
    ax3.set_ylabel('Y')
    ax3.set_zlabel('Z')
    ax3.legend()

    

    
    plt.show()

def main():
    # Read the data from text files
    planet_data = read_data('/Users/edward/Documents/SatelliteOrbitSimulation/data/planet_data.txt')
    satellite_data = read_data('/Users/edward/Documents/SatelliteOrbitSimulation/data/satellite_data.txt')
    
    # Plot the data
    plot_data(planet_data, satellite_data)

if __name__ == '__main__':
    main()
