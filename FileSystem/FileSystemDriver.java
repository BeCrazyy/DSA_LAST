import java.util.*;
import java.util.regex.PatternSyntaxException;

/**
 * FileService class implementing a file system API with mkdir, cd, and pwd operations.
 * 
 * Time Complexity:
 * - makeDirectory: O(1) average case for HashMap operations
 * - changeDirectory: O(n*m) where n is number of path components and m is subdirectories for regex matching
 * - printWorkingDirectory: O(1)
 * 
 * Space Complexity: O(d) where d is total number of directories created
 */
class FileService {
    // Maps directory paths to their immediate subdirectories
    private final Map<String, Set<String>> directories;
    // Current working directory path
    private String currentPath;

    /**
     * Constructor initializes the file system with /home as root directory.
     */
    public FileService() {
        this.currentPath = "/home"; // Set initial working directory
        this.directories = new HashMap<>(); // Initialize directory structure
        this.directories.put("/home", new HashSet<>()); // Create root directory with empty subdirectory set
    }

    /**
     * Creates a new directory in the current working directory.
     * Time Complexity: O(1) average case
     * 
     * @param directoryName Name of the directory to create
     */
    public void makeDirectory(String directoryName) {
        // Build full path for new directory by combining current path with directory name
        String newDirectoryPath = buildPath(currentPath, directoryName);
        
        // Check if directory doesn't already exist to avoid duplicates
        if (!directories.containsKey(newDirectoryPath)) {
            // Create new directory entry with empty subdirectory set
            directories.put(newDirectoryPath, new HashSet<>());
            // Add directory name to parent's subdirectory list
            directories.get(currentPath).add(directoryName); 
        }
    }

    /**
     * Changes the current working directory to the specified target.
     * Supports absolute paths, relative paths, regex patterns, and special directories (. and ..).
     * Time Complexity: O(n*m) where n is path components, m is subdirectories for regex matching
     * 
     * @param targetDirectory Target directory path (can include regex patterns)
     */
    public void changeDirectory(String targetDirectory) {
        // Handle absolute paths (starting with /)
        if (targetDirectory.startsWith("/")) {
            // Check if absolute path exists in directory structure
            if (directories.containsKey(targetDirectory)) {
                currentPath = targetDirectory; // Update current path directly
            }
            return; // Exit early for absolute paths
        }

        // Split relative path into individual components for processing
        List<String> pathComponents = Arrays.asList(targetDirectory.split("/"));
        String tempPath = currentPath; // Use temporary path for validation

        // Process each path component sequentially
        for (String component : pathComponents) {
            tempPath = processPathComponent(component, tempPath);
            if (tempPath == null) {
                return; // Invalid component found, abort navigation
            }
        }

        // All components processed successfully, update current path
        currentPath = tempPath;
    }

    /**
     * Returns the current working directory path.
     * Time Complexity: O(1)
     * 
     * @return Current working directory path
     */
    public String printWorkingDirectory() {
        return currentPath; // Simply return stored current path
    }

    /**
     * Processes individual path components during directory navigation.
     * 
     * @param component Path component to process
     * @param currentTempPath Current temporary path during navigation
     * @return New path after processing component, null if invalid
     */
    private String processPathComponent(String component, String currentTempPath) {
        // Skip empty components (caused by double slashes in path)
        if (component.isEmpty()) {
            return currentTempPath;
        }

        // Handle current directory reference
        if (component.equals(".")) {
            return currentTempPath; // Stay in same directory
        }

        // Handle parent directory reference
        if (component.equals("..")) {
            String parentPath = getParentPath(currentTempPath);
            // Return parent if exists, otherwise stay in current directory
            return parentPath != null ? parentPath : currentTempPath;
        }

        // Try exact directory name match first
        String nextPath = buildPath(currentTempPath, component);
        if (directories.containsKey(nextPath)) {
            return nextPath; // Found exact match
        }

        // If no exact match, try regex pattern matching
        return findRegexMatch(component, currentTempPath);
    }

    /**
     * Finds directory matching regex pattern in current directory.
     * Time Complexity: O(m) where m is number of subdirectories
     * 
     * @param pattern Regex pattern to match
     * @param currentTempPath Current path to search in
     * @return Path of first matching directory, null if no match
     */
    private String findRegexMatch(String pattern, String currentTempPath) {
        // Get list of subdirectories in current path
        Set<String> currentDirectories = directories.get(currentTempPath);
        
        if (currentDirectories != null) {
            // Iterate through each subdirectory name
            for (String directoryName : currentDirectories) {
                try {
                    // Check if directory name matches regex pattern
                    if (directoryName.matches(pattern)) {
                        // Return full path of first matching directory
                        return buildPath(currentTempPath, directoryName);
                    }
                } catch (PatternSyntaxException e) {
                    // Invalid regex pattern, continue to next directory
                    continue;
                }
            }
        }
        return null; // No matching directory found
    }

    /**
     * Gets parent directory path from given path.
     * 
     * @param path Directory path
     * @return Parent path, null if no parent exists
     */
    private String getParentPath(String path) {
        // Find last occurrence of slash to determine parent
        int lastSlashIndex = path.lastIndexOf('/');
        
        // If slash found and not at beginning, extract parent path
        if (lastSlashIndex > 0) {
            return path.substring(0, lastSlashIndex);
        }
        return null; // No parent exists (already at root)
    }

    /**
     * Builds full path by combining base path and directory name.
     * 
     * @param basePath Base directory path
     * @param directoryName Directory name to append
     * @return Combined full path
     */
    private String buildPath(String basePath, String directoryName) {
        // Concatenate base path with slash and directory name
        return basePath + "/" + directoryName;
    }
}

/**
 * Main driver class to demonstrate the File System API functionality.
 */
public class FileSystemDriver {
    public static void main(String[] args) {
        // Initialize file system service
        FileService fileSystem = new FileService();

        System.out.println("=== File System API Demo ===\n");

        // Test 1: Display initial working directory
        System.out.println("1. Initial working directory:");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 2: Create multiple directories in root
        System.out.println("2. Creating directories:");
        fileSystem.makeDirectory("Documents");
        fileSystem.makeDirectory("Downloads");
        fileSystem.makeDirectory("Desktop");
        System.out.println("Created: Documents, Downloads, Desktop");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 3: Navigate to Documents directory
        System.out.println("3. Navigate to Documents:");
        fileSystem.changeDirectory("Documents");
        System.out.println("cd Documents");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 4: Create subdirectories within Documents
        System.out.println("4. Creating subdirectories in Documents:");
        fileSystem.makeDirectory("DocFolder1");
        fileSystem.makeDirectory("DocFolder2");
        fileSystem.makeDirectory("ProjectFiles");
        System.out.println("Created: DocFolder1, DocFolder2, ProjectFiles");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 5: Test regex pattern matching for navigation
        System.out.println("5. Navigate using regex (Doc.*):");
        fileSystem.changeDirectory("Doc.*");
        System.out.println("cd Doc.*");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 6: Navigate to parent directory using ..
        System.out.println("6. Go back to parent directory:");
        fileSystem.changeDirectory("..");
        System.out.println("cd ..");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 7: Navigate to Downloads and create subdirectories
        System.out.println("7. Navigate to Downloads and create subdirectories:");
        fileSystem.changeDirectory("Downloads");
        fileSystem.makeDirectory("DowFolder1");
        fileSystem.makeDirectory("DowFolder2");
        fileSystem.makeDirectory("Software");
        System.out.println("cd Downloads");
        System.out.println("Created: DowFolder1, DowFolder2, Software");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 8: Test complex relative path navigation
        System.out.println("8. Complex path navigation (../../Documents/ProjectFiles):");
        fileSystem.changeDirectory("../../Documents/ProjectFiles");
        System.out.println("cd ../../Documents/ProjectFiles");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 9: Test absolute path navigation
        System.out.println("9. Navigate using absolute path:");
        fileSystem.changeDirectory("/home");
        System.out.println("cd /home");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 10: Test regex with character classes
        System.out.println("10. Navigate using regex with character classes ([A-Z].*):");
        fileSystem.changeDirectory("[A-Z].*");
        System.out.println("cd [A-Z].*");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 11: Test current directory navigation
        System.out.println("11. Test current directory navigation (./.):");
        fileSystem.changeDirectory("./.");
        System.out.println("cd ./.");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println();

        // Test 12: Test invalid regex pattern handling
        System.out.println("12. Test invalid regex pattern ([abc):");
        String beforeInvalidRegex = fileSystem.printWorkingDirectory();
        fileSystem.changeDirectory("[abc");
        System.out.println("cd [abc");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println("Stayed in same directory due to invalid regex: " +
                beforeInvalidRegex.equals(fileSystem.printWorkingDirectory()));
        System.out.println();

        // Test 13: Test non-existent directory handling
        System.out.println("13. Test non-existent directory (NonExistent):");
        String beforeNonExistent = fileSystem.printWorkingDirectory();
        fileSystem.changeDirectory("NonExistent");
        System.out.println("cd NonExistent");
        System.out.println("pwd: " + fileSystem.printWorkingDirectory());
        System.out.println("Stayed in same directory due to non-existent path: " +
                beforeNonExistent.equals(fileSystem.printWorkingDirectory()));

        System.out.println("\n=== File System API Demo Complete ===");
    }
}

/*
=== PROBLEM STATEMENT ===

File System API Implementation

You need to implement a file system API that mimics three fundamental Linux commands:

1. mkdir (Make Directory)
   * Create directories in the file system
   * Handle directory creation with proper path resolution
   * Manage parent directory relationships

2. cd (Change Directory)
   * Navigate between directories in the file system
   * Support path changes using directory names
   * Handle regex patterns in path specifications for flexible navigation
   * Support relative paths (., .., subdirectory names)
   * Support absolute paths (starting with /)
   * Update the current working directory context

3. pwd (Print Working Directory)
   * Display the current working directory path
   * Return the absolute path from root to current location
   * Provide clear indication of current position in directory hierarchy

Requirements:
- The implementation should maintain proper directory structure
- Handle path traversal with support for . (current) and .. (parent) directories
- Support regex-based path matching for the cd command
- Keep track of the current working directory state throughout operations
- Handle invalid paths gracefully by staying in current directory
- Support complex path navigation like ../../Documents/ProjectFiles

Algorithm Complexity:
- Time Complexity: O(n*m) for changeDirectory where n = path components, m = subdirectories for regex
- Space Complexity: O(d) where d = total number of directories created
- All operations use HashMap for O(1) average case directory lookups

Data Structure:
- Uses Map<String, Set<String>> to store directory structure
- Key: Full directory path, Value: Set of immediate subdirectory names
- Maintains currentPath string for tracking working directory

Implementation Details:
- HashMap provides O(1) average case lookup for directory existence checks
- Set ensures no duplicate subdirectory names within same parent
- String manipulation for path building and parent path extraction
- Regex pattern matching with exception handling for invalid patterns
- Sequential path component processing for complex relative paths
*/