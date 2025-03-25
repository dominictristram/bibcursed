#!/usr/bin/env python3
"""
Bibcursed - a curses-based BibTeX bibliography file editor
Copyright (C) 1999 - 2002  Dominic Tristram
Python port (C) 2024

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
"""

import curses
import sys
import os
import time
from typing import List, Dict, Optional, Tuple
import re

class Bibcursed:
    def __init__(self, bibfile: str):
        self.bibfile = bibfile
        self.rec_count = 0
        self.screen = None
        self.stdscr = None
        
    def setup_screen(self):
        """Clear screen and show header"""
        self.stdscr.clear()
        self.stdscr.addstr(0, 0, "bibcursed - Dominic Tristram, 1999 - 2025", curses.A_BOLD)
        self.stdscr.addstr(0, 41, f"       Entries in bibliography : {self.rec_count}")
        
    def add_bold(self, letter: str):
        """Add a bold letter at current position"""
        self.stdscr.addstr(letter, curses.A_BOLD)
        
    def menu(self):
        """Display and handle main menu"""
        self.setup_screen()
        
        # Menu options
        options = [
            (4, "Search and query", 'S'),
            (5, "Add a new entry", 'A'),
            (6, "Delete an entry", 'D'),
            (7, "View or Change an existing entry", 'V'),
            (8, "List all entries", 'L'),
            (11, "Help", 'H'),
            (12, "Quit", 'Q')
        ]
        
        for y, text, bold in options:
            self.stdscr.move(y, 5)
            if bold:
                self.add_bold(bold)
                self.stdscr.addstr(text[len(bold):])
            else:
                self.stdscr.addstr(text)
                
        self.stdscr.move(15, 5)
        self.stdscr.addstr("What do you want to do? ")
        self.stdscr.refresh()
        
        # Get user input
        ch = self.stdscr.getch()
        lch = chr(ch).lower()
        
        # Handle menu selection
        actions = {
            's': self.search_menu,
            'a': self.add_entry,
            'd': self.delete_entry,
            'v': self.change_entry,
            'l': self.list_all_entries,
            'h': self.help,
            'q': self.quit_prog
        }
        
        if lch in actions:
            actions[lch]()
            
    def help(self):
        """Display help screen"""
        self.setup_screen()
        help_text = [
            "This is the main bibcursed screen. You can choose from four main",
            "options. Press 's' to search for key words in your BibTeX file.",
            "You will be given a list of entry names containing the search",
            "term. You can then display these by using the 'C' option on",
            "this screen.",
            "",
            "The Add option will ask you for details about your new",
            "reference, then add it to the database. The delete option will",
            "remove a reference with the ID you supply from the database."
        ]
        
        for i, line in enumerate(help_text, start=4):
            self.stdscr.addstr(i, 0, line)
            
        self.stdscr.addstr(14, 5, "Press a key to continue, or H for more help")
        self.stdscr.refresh()
        
        ch = self.stdscr.getch()
        if chr(ch).lower() == 'h':
            self.general_help()
            
    def general_help(self):
        """Display general help screen"""
        self.setup_screen()
        help_text = [
            "Please refer to the man page and documentation provided",
            "with this distribution. Check the bibcursed website at:",
            "",
            "http://sourceforge.net/projects/bibcursed",
            "",
            "before sending any queries to me at:",
            "",
            "dominic@dominictristram.com"
        ]
        
        for i, line in enumerate(help_text, start=4):
            self.stdscr.addstr(i, 5, line)
            
        self.stdscr.addstr(17, 5, "Press any key to continue")
        self.stdscr.refresh()
        self.stdscr.getch()
        
    def quit_prog(self):
        """Clean up and exit program"""
        curses.endwin()
        sys.exit(0)
        
    def run(self):
        """Main program loop"""
        # Initialize curses
        self.stdscr = curses.initscr()
        curses.start_color()
        curses.use_default_colors()
        
        # Count entries in bibfile
        try:
            with open(self.bibfile, 'r') as f:
                self.rec_count = sum(1 for line in f if line.startswith('@'))
        except FileNotFoundError:
            # Create new file if it doesn't exist
            with open(self.bibfile, 'w') as f:
                pass
            self.rec_count = 0
            
        print(f"There are {self.rec_count} BibTeX entries in {self.bibfile}")
        time.sleep(1)
        
        # Main menu loop
        while True:
            self.menu()

    def search_menu(self):
        """Display the search menu and handle search options"""
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Search by :")
        self.stdscr.addstr(6, 8, "A. uthor")
        self.stdscr.addstr(7, 8, "P. rinciple author only")
        self.stdscr.addstr(8, 8, "L. inked authors")
        self.stdscr.addstr(9, 8, "T. itle")
        self.stdscr.addstr(10, 8, "J. ournal or conference title")
        self.stdscr.addstr(11, 8, "U. niversity or institution (for thesis)")
        self.stdscr.addstr(12, 8, "O. rganisation (for proceedings etc)")
        self.stdscr.addstr(13, 8, "Y. ear")
        self.stdscr.addstr(14, 8, "E. verything (keyword for all fields)")
        self.stdscr.addstr(15, 8, "Q. uit and return to main menu")
        self.stdscr.addstr(17, 8, "What now? ")
        self.stdscr.refresh()
        
        ch = self.stdscr.getch()
        lch = chr(ch).lower()
        
        search_type = {
            'a': 1,  # Author
            'p': 2,  # Principle author
            'l': 3,  # Linked authors
            't': 4,  # Title
            'j': 5,  # Journal/conference
            'u': 6,  # University/institution
            'y': 7,  # Year
            'e': 8,  # Everything (keyword)
            'o': 9,  # Organisation
            'q': 0   # Quit
        }
        
        if lch in search_type:
            if lch == 'q':
                return
            self.search_file(search_type[lch])
        else:
            self.menu()

    def search_file(self, search_type: int):
        """Search the BibTeX file based on the specified search type"""
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Please enter ")
        
        # Define search type descriptions and field names
        search_types = {
            1: ("author surname", "AUTHOR", "author"),
            2: ("principle author surname", "AUTHOR", "author"),
            3: ("surname of author to link to", "AUTHOR", "author"),
            4: ("title (or partial title)", "TITLE", "title"),
            5: ("conference or journal name", "JOURNAL", "journal"),
            6: ("name of university or institution", "SCHOOL", "school"),
            7: ("year", "YEAR", "year"),
            8: ("keyword", "all", "all"),
            9: ("name of organisation", "ORGANIZATION", "organization")
        }
        
        if search_type not in search_types:
            self.stdscr.addstr(4, 5, "Invalid search type")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        desc, field, field_lower = search_types[search_type]
        self.stdscr.addstr(4, 5, f"Please enter {desc} for search : ")
        self.stdscr.refresh()
        
        # Create input window
        input_win = curses.newwin(1, 50, 4, 40)
        curses.echo()
        search_term = input_win.getstr().decode('utf-8')
        curses.noecho()
        
        if not search_term:
            return
            
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Searching for ")
        self.stdscr.attron(curses.A_BOLD)
        self.stdscr.addstr(4, 20, search_term)
        self.stdscr.attroff(curses.A_BOLD)
        self.stdscr.refresh()
        
        try:
            with open(self.bibfile, 'r') as f:
                lines = f.readlines()
        except FileNotFoundError:
            self.stdscr.addstr(6, 5, f"Error: Could not find file {self.bibfile}")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        print_pos = 6
        entries_found = 0
        total_entries = 0
        current_entry = []
        in_entry = False
        
        for line in lines:
            line = line.strip()
            if not line:
                continue
                
            if line.startswith('@'):
                if in_entry:
                    # Process previous entry
                    entry_text = '\n'.join(current_entry)
                    if self._entry_matches_search(entry_text, search_term, field, field_lower):
                        self._display_entry(entry_text, print_pos)
                        print_pos += len(current_entry)
                        entries_found += 1
                        total_entries += 1
                        
                        if entries_found == ((curses.LINES - 9) // 2):
                            self._handle_pagination(print_pos, search_term)
                            entries_found = 0
                            print_pos = 6
                            
                # Start new entry
                current_entry = [line]
                in_entry = True
            elif in_entry:
                current_entry.append(line)
                
        # Process last entry
        if in_entry and current_entry:
            entry_text = '\n'.join(current_entry)
            if self._entry_matches_search(entry_text, search_term, field, field_lower):
                self._display_entry(entry_text, print_pos)
                total_entries += 1
                
        self.stdscr.addstr(print_pos + 1, 5, f"That's it - {total_entries} match. Press any key")
        self.stdscr.refresh()
        self.stdscr.getch()
        
    def _entry_matches_search(self, entry_text: str, search_term: str, field: str, field_lower: str) -> bool:
        """Check if an entry matches the search criteria"""
        if field == "all":
            return search_term.lower() in entry_text.lower()
            
        # Look for the field in the entry
        field_pattern = f"{field} = {{"
        field_pattern_lower = f"{field_lower} = {{"
        
        if field_pattern not in entry_text and field_pattern_lower not in entry_text:
            return False
            
        # Extract the field value
        start = entry_text.find(field_pattern)
        if start == -1:
            start = entry_text.find(field_pattern_lower)
            
        start = entry_text.find("{", start) + 1
        end = entry_text.find("}", start)
        if end == -1:
            return False
            
        field_value = entry_text[start:end]
        return search_term.lower() in field_value.lower()
        
    def _display_entry(self, entry_text: str, print_pos: int):
        """Display a matching entry at the specified position"""
        for i, line in enumerate(entry_text.split('\n')):
            if i == 0:
                self.stdscr.addstr(print_pos + i, 5, line)
            else:
                self.stdscr.addstr(print_pos + i, 8, line)
        self.stdscr.refresh()
        
    def _handle_pagination(self, print_pos: int, search_term: str):
        """Handle pagination of search results"""
        self.stdscr.addstr(print_pos + 1, 5, "Press any key for more results")
        self.stdscr.refresh()
        self.stdscr.getch()
        
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Searching for ")
        self.stdscr.attron(curses.A_BOLD)
        self.stdscr.addstr(4, 20, search_term)
        self.stdscr.attroff(curses.A_BOLD)
        self.stdscr.refresh()

    def add_entry(self):
        """Add a new BibTeX entry"""
        self.setup_screen()
        
        # Entry type options
        entry_types = [
            (6, "1. Article - paper or article in journal", '1'),
            (7, "2. Book - whole book", '2'),
            (8, "3. Booklet - bound work with no publisher name", '3'),
            (9, "4. Conference / in proceedings - paper or article from proceedings", '4'),
            (10, "5. In book - extract from a book", '5'),
            (11, "6. In collection - part of book with its own title", '6'),
            (12, "7. Manual - technical documentation", '7'),
            (13, "8. Masters thesis", '8'),
            (14, "9. Miscellaneous - anything not covered by other options", '9'),
            (15, "A. PhD thesis", 'a'),
            (16, "B. Proceedings - whole proceedings", 'b'),
            (17, "C. Technical report - report published by institution", 'c'),
            (18, "D. Unpublished - unpublished work with author and title", 'd'),
            (20, "Q. None of these! Go back!", 'q')
        ]
        
        for y, text, bold in entry_types:
            self.stdscr.move(y, 5)
            if bold:
                self.add_bold(bold)
                self.stdscr.addstr(text[len(bold):])
            else:
                self.stdscr.addstr(text)
                
        self.stdscr.refresh()
        
        # Get entry type
        ch = self.stdscr.getch()
        entry_type = chr(ch).lower()
        
        if entry_type == 'q':
            return
            
        # Define required fields for each entry type
        required_fields = {
            '1': {'author': True, 'title': True, 'journal': True, 'year': True},
            '2': {'author': True, 'title': True, 'publisher': True, 'year': True},
            '3': {'title': True},
            '4': {'author': True, 'title': True, 'booktitle': True, 'year': True},
            '5': {'author': True, 'title': True, 'chapter': True, 'pages': True, 'publisher': True, 'year': True},
            '6': {'author': True, 'title': True, 'booktitle': True, 'publisher': True, 'year': True},
            '7': {'title': True},
            '8': {'author': True, 'title': True, 'school': True, 'year': True},
            '9': {},
            'a': {'author': True, 'title': True, 'school': True, 'year': True},
            'b': {'title': True, 'year': True},
            'c': {'author': True, 'title': True, 'institution': True, 'year': True},
            'd': {'author': True, 'title': True, 'note': True}
        }
        
        # Get entry fields
        fields = {}
        while True:
            self.setup_screen()
            self.stdscr.addstr(3, 5, "Bold fields are required and must be used - others are optional")
            self.stdscr.addstr(4, 5, "Fields marked with a  *  have been set. Press Z when done or")
            self.stdscr.addstr(5, 5, "Q to quit")
            
            # Display field options
            field_options = [
                ('address', '1. Address'),
                ('annote', '2. Annote'),
                ('author', '3. Author(s)'),
                ('booktitle', '4. Booktitle'),
                ('chapter', '5. Chapter'),
                ('crossref', '6. Cross-reference'),
                ('edition', '7. Edition'),
                ('editor', '8. Editor(s)'),
                ('howpublished', '9. Howpublished'),
                ('institution', 'A. Institution'),
                ('journal', 'B. Journal'),
                ('key', 'C. Key'),
                ('month', 'D. Month'),
                ('note', 'E. Note'),
                ('number', 'F. Number'),
                ('organization', 'G. Organisation'),
                ('pages', 'H. Pages'),
                ('publisher', 'I. Publisher'),
                ('school', 'J. School'),
                ('series', 'K. Series'),
                ('title', 'L. Title'),
                ('type', 'M. Type'),
                ('volume', 'N. Volume'),
                ('year', 'O. Year')
            ]
            
            y = 7
            for field, text in field_options:
                self.stdscr.move(y, 5)
                is_required = required_fields.get(entry_type, {}).get(field, False)
                if is_required:
                    self.stdscr.addstr(text, curses.A_BOLD)
                else:
                    self.stdscr.addstr(text)
                if field in fields:
                    self.stdscr.addstr(" *")
                y += 1
                
            self.stdscr.refresh()
            
            # Get field selection
            ch = self.stdscr.getch()
            lch = chr(ch).lower()
            
            if lch == 'q':
                return
            elif lch == 'z':
                # Check if all required fields are set
                missing = [field for field, required in required_fields.get(entry_type, {}).items() 
                          if required and field not in fields]
                if missing:
                    self.stdscr.addstr(10, 5, f"You can't finish yet - there are still {len(missing)} required fields missing!")
                    self.stdscr.addstr(12, 5, "Press a key")
                    self.stdscr.refresh()
                    self.stdscr.getch()
                    continue
                break
                
            # Handle field selection
            field_map = {
                '1': 'address', '2': 'annote', '3': 'author', '4': 'booktitle',
                '5': 'chapter', '6': 'crossref', '7': 'edition', '8': 'editor',
                '9': 'howpublished', 'a': 'institution', 'b': 'journal',
                'c': 'key', 'd': 'month', 'e': 'note', 'f': 'number',
                'g': 'organization', 'h': 'pages', 'i': 'publisher',
                'j': 'school', 'k': 'series', 'l': 'title', 'm': 'type',
                'n': 'volume', 'o': 'year'
            }
            
            if lch in field_map:
                field = field_map[lch]
                self.setup_screen()
                self.stdscr.addstr(4, 5, f"Please enter the {field}")
                self.stdscr.refresh()
                
                # Create input window
                input_win = curses.newwin(1, 50, 4, 40)
                curses.echo()
                value = input_win.getstr().decode('utf-8')
                curses.noecho()
                
                if value:
                    fields[field] = value
                    
        # Get entry name
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Please enter an entry name for the new reference")
        self.stdscr.refresh()
        
        input_win = curses.newwin(1, 50, 4, 40)
        curses.echo()
        entry_name = input_win.getstr().decode('utf-8')
        curses.noecho()
        
        if not entry_name:
            return
            
        # Create new entry
        entry_type_map = {
            '1': 'article', '2': 'book', '3': 'booklet', '4': 'inproceedings',
            '5': 'inbook', '6': 'incollection', '7': 'manual', '8': 'mastersthesis',
            '9': 'misc', 'a': 'phdthesis', 'b': 'proceedings', 'c': 'techreport',
            'd': 'unpublished'
        }
        
        # Read existing entries
        try:
            with open(self.bibfile, 'r') as f:
                entries = f.readlines()
        except FileNotFoundError:
            entries = []
            
        # Create new entry text
        new_entry = [f"@{entry_type_map[entry_type]}{{{entry_name}"]
        for field, value in fields.items():
            new_entry.append(f"  {field} = {{{value}}}")
        new_entry.append("}\n\n")
        
        # Insert new entry in alphabetical order
        inserted = False
        for i, line in enumerate(entries):
            if line.startswith('@'):
                if line > new_entry[0]:
                    entries[i:i] = new_entry
                    inserted = True
                    break
                    
        if not inserted:
            entries.extend(new_entry)
            
        # Write back to file
        try:
            # Backup original file
            os.rename(self.bibfile, f"{self.bibfile}.old")
            
            with open(self.bibfile, 'w') as f:
                f.writelines(entries)
                
            self.rec_count += 1
            self.stdscr.addstr(7, 5, "New entry was successfully written. Press a key")
            
        except Exception as e:
            self.stdscr.addstr(7, 5, f"Error writing entry: {str(e)}")
            
        self.stdscr.refresh()
        self.stdscr.getch()

    def delete_entry(self):
        """Delete a BibTeX entry"""
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Enter the name of the entry to delete")
        self.stdscr.refresh()
        
        # Create input window
        input_win = curses.newwin(1, 50, 4, 40)
        curses.echo()
        entry_name = input_win.getstr().decode('utf-8')
        curses.noecho()
        
        if not entry_name:
            return
            
        # Read file and find entry
        try:
            with open(self.bibfile, 'r') as f:
                lines = f.readlines()
        except FileNotFoundError:
            self.stdscr.addstr(6, 5, f"Error: Could not find file {self.bibfile}")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        # Find and remove entry
        new_lines = []
        in_entry = False
        entry_found = False
        
        for line in lines:
            if line.startswith('@'):
                # Check if this is the entry to delete
                if entry_name in line:
                    in_entry = True
                    entry_found = True
                    continue
                elif in_entry:
                    in_entry = False
                    continue
                    
            if not in_entry:
                new_lines.append(line)
                
        if not entry_found:
            self.stdscr.addstr(7, 5, "Record not found - nothing removed")
        else:
            # Backup original file
            try:
                os.rename(self.bibfile, f"{self.bibfile}.old")
                
                # Write new file
                with open(self.bibfile, 'w') as f:
                    f.writelines(new_lines)
                    
                self.rec_count -= 1
                self.stdscr.addstr(7, 5, f"{entry_name} removed")
                
            except Exception as e:
                self.stdscr.addstr(7, 5, f"Error deleting entry: {str(e)}")
                
        self.stdscr.addstr(10, 5, "Press any key")
        self.stdscr.refresh()
        self.stdscr.getch()

    def change_entry(self):
        """View or change an existing BibTeX entry"""
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Please enter the record name : ")
        self.stdscr.refresh()
        
        # Create input window
        input_win = curses.newwin(1, 50, 4, 40)
        curses.echo()
        entry_name = input_win.getstr().decode('utf-8')
        curses.noecho()
        
        if not entry_name:
            return
            
        # Read file and find entry
        try:
            with open(self.bibfile, 'r') as f:
                lines = f.readlines()
        except FileNotFoundError:
            self.stdscr.addstr(6, 5, f"Error: Could not find file {self.bibfile}")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        # Find and display entry
        print_pos = 6
        entry_found = False
        record_count = 0
        
        for line in lines:
            if line.startswith('@'):
                if entry_name in line:
                    if record_count > 0:
                        self.stdscr.addstr(print_pos, 5, "More than one match! Be more specific! Any key for next match")
                        self.stdscr.refresh()
                        self.stdscr.getch()
                        print_pos = 6
                        self.setup_screen()
                        self.stdscr.addstr(4, 0)
                        self.stdscr.refresh()
                    record_count += 1
                    entry_found = True
                    self.stdscr.addstr(print_pos, 5, line.strip())
                    print_pos += 1
                    continue
                    
            if entry_found and record_count == 1:
                if line.strip() and not line.startswith('@'):
                    self.stdscr.addstr(print_pos, 5, line.strip())
                    print_pos += 1
                elif line.startswith('@'):
                    break
                    
        if not entry_found:
            self.stdscr.addstr(print_pos + 1, 5, "The record name was not found! Press a key")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        if record_count > 1:
            self.stdscr.addstr(print_pos + 1, 5, "You must give a unique ID to change a field - try again")
        else:
            self.stdscr.addstr(print_pos + 1, 5, "Press C to change a field or any other key to return to the menu")
            
        self.stdscr.refresh()
        ch = self.stdscr.getch()
        
        if chr(ch).lower() == 'c':
            self.change_field(entry_name)
            
    def change_field(self, entry_name: str):
        """Change a field in an existing BibTeX entry"""
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Select a field to add/replace.")
        self.stdscr.addstr(5, 5, "Note that not all are valid for every entry type!")
        self.stdscr.addstr(6, 5, "If in doubt, don't add anything that isn't there already")
        
        # Field options
        field_options = [
            (8, "1. Address          D. Month"),
            (9, "2. Annote           E. Note"),
            (10, "3. Author           F. Number"),
            (11, "4. Booktitle        G. Organisation"),
            (12, "5. Chapter          H. Pages"),
            (13, "6. Crossref         I. Publisher"),
            (14, "7. Edition          J. School"),
            (15, "8. Editor           K. Series"),
            (16, "9. Howpublished     L. Title"),
            (17, "A. Institution      M. Type"),
            (18, "B. Journal          N. Volume"),
            (19, "C. Key              O. Year")
        ]
        
        for y, text in field_options:
            self.stdscr.addstr(y, 8, text)
            
        self.stdscr.addstr(21, 8, "Make your choice or press q to quit")
        self.stdscr.refresh()
        
        ch = self.stdscr.getch()
        lch = chr(ch).upper()
        
        if lch == 'Q':
            return
            
        # Field mapping
        field_map = {
            '1': ('ADDRESS', 'address'),
            '2': ('ANNOTE', 'annote'),
            '3': ('AUTHOR', 'author'),
            '4': ('BOOKTITLE', 'booktitle'),
            '5': ('CHAPTER', 'chapter'),
            '6': ('CROSSREF', 'crossref'),
            '7': ('EDITION', 'edition'),
            '8': ('EDITOR', 'editor'),
            '9': ('HOWPUBLISHED', 'howpublished'),
            'A': ('INSTITUTION', 'institution'),
            'B': ('JOURNAL', 'journal'),
            'C': ('KEY', 'key'),
            'D': ('MONTH', 'month'),
            'E': ('NOTE', 'note'),
            'F': ('NUMBER', 'number'),
            'G': ('ORGANIZATION', 'organization'),
            'H': ('PAGES', 'pages'),
            'I': ('PUBLISHER', 'publisher'),
            'J': ('SCHOOL', 'school'),
            'K': ('SERIES', 'series'),
            'L': ('TITLE', 'title'),
            'M': ('TYPE', 'type'),
            'N': ('VOLUME', 'volume'),
            'O': ('YEAR', 'year')
        }
        
        if lch not in field_map:
            return
            
        field_name, field_name_lower = field_map[lch]
        
        # Get new field value
        self.setup_screen()
        self.stdscr.addstr(4, 5, f"Enter data for {field_name_lower} and press return to finish")
        self.stdscr.addstr(5, 5, "To delete the field, just press return")
        self.stdscr.refresh()
        
        input_win = curses.newwin(1, 50, 7, 1)
        curses.echo()
        field_data = input_win.getstr().decode('utf-8')
        curses.noecho()
        
        # Read file and update entry
        try:
            with open(self.bibfile, 'r') as f:
                lines = f.readlines()
        except FileNotFoundError:
            self.stdscr.addstr(10, 5, f"Error: Could not find file {self.bibfile}")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        # Process file
        new_lines = []
        in_entry = False
        field_changed = False
        
        for line in lines:
            if line.startswith('@'):
                if entry_name in line:
                    in_entry = True
                    new_lines.append(line)
                    if field_data:
                        new_lines.append(f"  {field_name} = {{{field_data}}},\n")
                    field_changed = True
                    continue
                elif in_entry:
                    in_entry = False
                    
            if not in_entry or (in_entry and not (field_name in line or field_name_lower in line)):
                new_lines.append(line)
                
        if field_changed:
            # Backup original file
            try:
                os.rename(self.bibfile, f"{self.bibfile}.old")
                
                # Write new file
                with open(self.bibfile, 'w') as f:
                    f.writelines(new_lines)
                    
                self.stdscr.addstr(10, 5, "Change made - press a key")
            except Exception as e:
                self.stdscr.addstr(10, 5, f"Error making change: {str(e)}")
        else:
            self.stdscr.addstr(10, 5, "No changes made - press a key")
            
        self.stdscr.refresh()
        self.stdscr.getch()

    def list_all_entries(self):
        """Display all entries in the BibTeX file with nice formatting"""
        self.setup_screen()
        self.stdscr.addstr(4, 5, "Loading entries...")
        self.stdscr.refresh()
        
        try:
            with open(self.bibfile, 'r') as f:
                lines = f.readlines()
        except FileNotFoundError:
            self.stdscr.addstr(6, 5, f"Error: Could not find file {self.bibfile}")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        # Parse entries
        entries = []
        current_entry = []
        for line in lines:
            line = line.strip()
            if not line:
                continue
                
            if line.startswith('@'):
                if current_entry:
                    entries.append('\n'.join(current_entry))
                current_entry = [line]
            else:
                current_entry.append(line)
                
        if current_entry:
            entries.append('\n'.join(current_entry))
            
        if not entries:
            self.stdscr.addstr(6, 5, "No entries found in the file")
            self.stdscr.refresh()
            self.stdscr.getch()
            return
            
        # Get screen dimensions
        max_y, max_x = self.stdscr.getmaxyx()
        entries_per_page = max(1, (max_y - 8) // 3)  # More conservative estimate
        
        # Display entries with pagination
        current_pos = 0
        while True:
            try:
                self.setup_screen()
                
                # Display entries
                display_pos = 7
                entries_displayed = 0
                
                for i in range(entries_per_page):
                    if current_pos + i >= len(entries):
                        break
                        
                    entry = entries[current_pos + i]
                    entry_lines = entry.split('\n')
                    
                    # Check if we have enough space for this entry
                    if display_pos + len(entry_lines) >= max_y - 1:
                        break
                        
                    # Format and display entry
                    for j, line in enumerate(entry_lines):
                        if display_pos + j >= max_y - 1:
                            break
                            
                        try:
                            if j == 0:
                                # Entry type line
                                self.stdscr.addstr(display_pos + j, 5, line[:max_x-6], curses.A_BOLD)
                            else:
                                # Field line
                                if '=' in line:
                                    field, value = line.split('=', 1)
                                    field = field.strip()[:15]  # Limit field width
                                    value = value.strip()[:max_x-30]  # Limit value width
                                    self.stdscr.addstr(display_pos + j, 5, field, curses.A_BOLD)
                                    self.stdscr.addstr(display_pos + j, 25, value)
                                else:
                                    self.stdscr.addstr(display_pos + j, 5, line[:max_x-6])
                        except curses.error:
                            # Skip if we can't write to this position
                            continue
                            
                    display_pos += len(entry_lines) + 1
                    entries_displayed += 1
                
                # Update status line after we know how many entries were actually displayed
                self.stdscr.addstr(4, 5, f"Showing entries {current_pos + 1} to {current_pos + entries_displayed} of {len(entries)}")
                
                # Check if we're at the last record
                is_last_record = current_pos + entries_displayed >= len(entries)
                if is_last_record:
                    self.stdscr.addstr(5, 5, "Last record - Press P for previous page, or Q to quit")
                else:
                    self.stdscr.addstr(5, 5, "Press N for next page, P for previous page, or Q to quit")
                    
                self.stdscr.refresh()
                
                # Handle navigation
                while True:
                    ch = self.stdscr.getch()
                    lch = chr(ch).lower()
                    
                    if lch == 'n' and not is_last_record:
                        current_pos += entries_displayed
                        break
                    elif lch == 'p' and current_pos > 0:
                        current_pos = max(0, current_pos - entries_per_page)
                        break
                    elif lch == 'q':
                        return
                        
            except curses.error:
                # If we get a curses error, try to recover
                self.stdscr.clear()
                self.stdscr.refresh()
                continue

def main():
    if len(sys.argv) != 2:
        print("Please enter a file name: ", end='')
        bibfile = input().strip()
    else:
        bibfile = sys.argv[1]
        
    app = Bibcursed(bibfile)
    try:
        app.run()
    except KeyboardInterrupt:
        curses.endwin()
        sys.exit(0)

if __name__ == "__main__":
    main() 