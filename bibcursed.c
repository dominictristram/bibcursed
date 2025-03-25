/*  Bibcursed - a curses-based BibTeX bibliography file editor
    Copyright (C) 1999 - 2002  Dominic Tristram

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Bibcursed Homepage - http://sourceforge.net/projects/bibcursed
    Dominic Tristram - dominic@dominictristram.com - http://www.dominictristram.com/

    I apologise for the code - it's a bit crap and messy... */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>

int rec_count;
char bibfile[100];

/* Function prototypes */
void add_bold(char letter);
int help();
int search_menu();
int search_file(int what);
int add_entry();
int delete_entry();
int change_entry();
int change_field(char *entry_name);
int list_all_entries();
int quit_prog();
int config_options();
void general_help();
int field_menu(int priority, int field_set, char *fieldname);

void setup_screen()
 {
   clear();
   move(0,0);
   attron(A_BOLD);
   printw("%s", "bibcursed - Dominic Tristram, 1999 - 2025");
   attroff(A_BOLD);
   printw("%s %d","       Entries in bibliography :", rec_count);
 }


void menu()
 {
   int ch, lch;

   setup_screen();
   move(4,5);
   add_bold('S');
   printw("%s","earch and query");
   move(5,5);
   add_bold('A');
   printw("%s","dd a new entry");
   move(6,5);
   add_bold('D');
   printw("%s","elete an entry");
   move(7,5);
   printw("%s","View or ");
   add_bold('C');
   printw("%s","hange an existing entry");
   move(8,5);
   add_bold('L');
   printw("%s","ist all entries");
   move(11,5);
   add_bold('H');
   printw("%s","elp");
   move(12,5);
   add_bold('Q');
   printw("%s","uit");
   move(15,5);
   printw("What do you want to do? ");

   refresh();
   cbreak();

   ch = getch();
   lch = tolower(ch);

   switch (lch)
    {
      case 's' : search_menu(); break;
      case 'a' : add_entry(); break;
      case 'd' : delete_entry(); break;
      case 'c' : change_entry(); break;
      case 'l' : list_all_entries(); break;
      case 'o' : config_options(); break;
      case 'h' : help(); break;
      case 'q' : quit_prog(); break;
      default : break;
    }
 }


int help()
 {
   char tmp_char, lch;

   setup_screen();
   move(4,0);
   printw("%s\n","This is the main bibcursed screen. You can choose from four main");
   printw("%s\n","options. Press 's' to search for key words in your BibTeX file.");
   printw("%s\n","You will be given a list of entry names containing the search");
   printw("%s\n","term. You can then display these by using the 'C' option on");
   printw("%s\n\n","this screen.");
   printw("%s\n","The Add option will ask you for details about your new");
   printw("%s\n","reference, then add it to the database. The delete option will");
   printw("%s\n","remove a reference with the ID you supply from the database.");
   move(14,5);
   printw("%s","Press a key to continue, or H for more help");
   refresh();
   cbreak();
   tmp_char = getch();
   lch = tolower(tmp_char);
   if (lch == 'h') general_help();
   return 0;
 }


void general_help()
 {
   char tmp_char;

   setup_screen();
   move(4,5);
   printw("%s","Please refer to the man page and documentation provided");
   move(5,5);
   printw("%s","with this distribution. Check the bibcursed website at:");
   move(7,8);
   printw("%s","https://github.com/dominictristram/bibcursed");
   move(9,5);
   printw("%s","before sending any queries to me at:");
   move(11,8);
   printw("%s","dominic@dominictristram.com");
  
   move(17,5);
   printw("%s","Press any key to continue");
   refresh();
   cbreak();
   tmp_char = getch();

 }


int config_options()
 {
   /* For future use */
   return 0;
 }

int add_entry()
 {
   char ch, lch, entry_type, length;
   int use_author = 0, use_title = 0, use_journal = 0, use_year = 0;
   int use_volume = 0, use_number = 0, use_series = 0, use_address = 0;
   int use_pages = 0, use_month = 0, use_note = 0, use_editor = 0;
   int use_publisher = 0, use_edition = 0, use_howpublished = 0, use_type =0;
   int use_booktitle = 0, use_chapter = 0, use_organisation = 0;
   int use_school = 0, use_institution = 0, use_annote = 0, use_key = 0;
   int set_address = 0, set_annote = 0, set_author = 0, set_booktitle = 0;
   int set_chapter = 0, set_crossref = 0, set_edition = 0, set_editor = 0;
   int set_howpublished = 0, set_institution = 0, set_journal = 0;
   int set_key = 0, set_month = 0, set_note = 0, set_number = 0;
   int set_organisation = 0, set_pages = 0, set_publisher = 0, set_school = 0;
   int set_series = 0, set_title = 0, set_type = 0, set_volume = 0;
   int set_year = 0, not_done=0;
   int print_pos = 6;
   int still_setting_up = 0, written_new=0;
   int entry_count = 0;
   char address[100], annote[100], author[100], booktitle[100], chapter[10], crossref[100], edition[100];
   char editor[100], howpublished[100], institution[100], journal[200], key[50], month[15];
   char note[400], number[10], organisation[100], pages[10], publisher[100], school[100], series[100];
   char title[200], type[100], volume[10], year[5];
   char entry_name[100], out_string[100], str[100], str2[100], *key_name;
   FILE *fin, *fout;

   setup_screen();
   move(4,5);
   printw("%s","Please choose entry type");
   move(6,8);
   printw("%s","1. Article - paper or article in journal");
   move(7,8);
   printw("%s","2. Book - whole book");
   move(8,8);
   printw("%s","3. Booklet - bound work with no publisher name");
   move(9,8);
   printw("%s","4. Conference / in proceedings - paper or article from proceedings");
   move(10,8);
   printw("%s","5. In book - extract from a book");
   move(11,8);
   printw("%s","6. In collection - part of book with its own title");
   move(12,8);
   printw("%s","7. Manual - technical documentation");
   move(13,8);
   printw("%s","8. Masters thesis");
   move(14,8);
   printw("%s","9. Miscellaneous - anything not covered by other options");
   move(15,8);
   printw("%s","A. PhD thesis");
   move(16,8);
   printw("%s","B. Proceedings - whole proceedings");
   move(17,8);
   printw("%s","C. Technical report - report published by institution");
   move(18,8);
   printw("%s","D. Unpublished - unpublished work with author and title");
   move(20,8);
   printw("%s","Q. None of these! Go back!");

   refresh();
   cbreak();
   ch = getch();
   entry_type = tolower(ch);

   if (entry_type == 'q') return(0);

   /* Set which fields to prompt for */
   switch(entry_type)
    {
      case '1' : use_author = 1; use_title = 1; use_journal = 1; use_year = 1;
                 use_volume = 2; use_number = 2; use_pages = 2;
                 use_month = 2; use_note = 2; break;
      case '2' : use_author = 1; use_editor = 2; use_title = 1; 
                 use_publisher = 1; use_year = 1; use_volume = 2;
                 use_number = 2; use_series = 2; use_address = 2;
                 use_edition = 2; use_month = 2; use_note = 2; break;
      case '3' : use_title = 1; use_author = 2; use_howpublished = 2;
                 use_address = 2; use_month = 2; use_year = 2;
                 use_note = 2; break;
      case '4' : use_author = 1; use_title = 1; use_booktitle = 1; use_year =1;
                 use_editor = 2; use_volume = 2; use_number = 2; use_series =2;
                 use_pages = 2; use_address = 2; use_month = 2; 
                 use_organisation = 2; use_publisher = 2; use_note = 2; break;
      case '5' : use_author = 1; use_editor = 1; use_title = 1;
                 use_chapter =1; use_pages = 1; use_publisher = 1;
                 use_year = 1; use_volume = 2; use_number = 2; use_series=2;
                 use_type = 2; use_address = 2; use_edition = 2; 
                 use_month = 2; use_note = 2; break;
      case '6' : use_author = 1; use_title = 1; use_booktitle = 1;
                 use_publisher = 1; use_year = 1; use_editor = 1;
                 use_chapter = 1; use_pages = 2; use_address = 2;
                 use_month = 2; use_note = 2; break;
      case '7' : use_title = 1; use_author =2; use_organisation=2;
                 use_address=2; use_edition=2; use_month=2; use_year=2;
                 use_note=2; break;
      case '8' : use_author=2; use_title=1; use_school=1; use_year=1;
                 use_type=2; use_address=2; use_month=2; use_note=2; break;
      case '9' : use_author=2; use_title=2; use_howpublished=2; use_month=2;
                 use_year=2; use_note=2; break;
      case 'a' : use_author=1; use_title=1; use_school=1; use_year=1;
                 use_type=2; use_address=2; use_month=2; use_note=2; break;
      case 'b' : use_title=1; use_year=1; use_editor=2; use_volume=2;
                 use_number=2; use_series=2; use_address=2; use_month=2;
                 use_organisation=2; use_publisher=2; use_note=2; break;
      case 'c' : use_author=1; use_title=1; use_institution=1; use_year=1;
                 use_type=2; use_number=2; use_address=2; use_month=2;
                 use_note=2; break;
      case 'd' : use_author=1; use_title=1; use_note=1; use_month=2;
                 use_year=2; break;
      default : return(0); break;
    }

   while (still_setting_up == 0)
    {
      print_pos = 7;
      setup_screen();
      move(3,5);
      attron(A_BOLD);
      printw("%s","Bold");
      attroff(A_BOLD);
      printw("%s"," fields are required and must be used - others are optional");
      move(4,5);
      printw("%s","Fields marked with a  *  have been set. Press Z when done or");
      move(5,5); printw("%s","Q to quit");
                
      if (use_address != 0)
       {
         move(print_pos,5);
         if (use_address == 1)
          {
            attron(A_BOLD);
            printw("%s","1. Address ");
            attroff(A_BOLD);
          }
         else
            printw("%s","1. Address ");
         if (set_author == 1) addch('*');
         print_pos++;
       }
      if (use_annote != 0)
       {
         move(print_pos,5);
         if (use_annote == 1)
          {
            attron(A_BOLD);
            printw("%s","2. Annote ");
            attroff(A_BOLD);
          }
         else
            printw("%s","2. Annote ");
         if (set_annote == 1) addch('*');
         print_pos++;
       }
      if (use_author != 0)
       {
         move(print_pos,5);
         if (use_author == 1)
          {
            attron(A_BOLD);
            printw("%s","3. Author(s) ");
            attroff(A_BOLD);
          }
         else
            printw("%s","3. Author(s) ");
         if (set_author == 1) addch('*');
         print_pos++;
       }
      if (use_booktitle != 0)
       {
         move(print_pos, 5);
         if (use_booktitle == 1)
          {
            attron(A_BOLD);
            printw("%s","4. Booktitle ");
            attroff(A_BOLD);
          }
         else
            printw("%s","4. Booktitle ");
         if (set_booktitle == 1) addch('*');
         print_pos++;
       }
      if (use_chapter != 0)
       {
         move(print_pos,5);
         if (use_chapter == 1)
          {
            attron(A_BOLD);
            printw("%s","5. Chapter ");
            attroff(A_BOLD);
          }
         else
            printw("%s","5. Chapter ");
         if (set_chapter == 1) addch('*');
         print_pos++;
       }
      move(print_pos,5);
      printw("%s","6. Cross-reference ");
      if (set_crossref == 1) addch('*');
      print_pos++;
      if (use_edition != 0)
       {
         move(print_pos,5);
         if (use_edition != 1)
          {
            attron(A_BOLD);
            printw("%s","7. Edition ");
            attroff(A_BOLD);
          }
         else
            printw("%s","7. Edition ");
         if (set_edition == 1) addch('*');
         print_pos++;
       }
      if (use_editor != 0)
       {
         move(print_pos,5);
         if (use_editor == 1)
          {
            attron(A_BOLD);
            printw("%s","8. Editor(s) ");
            attroff(A_BOLD);
          }
         else
            printw("%s","8. Editor(s) ");
         if (set_editor == 1) addch('*');
         print_pos++;
       }
      if (use_howpublished != 0)
       {
         move(print_pos,5);
         if (use_howpublished == 1)
          {
            attron(A_BOLD);
            printw("%s","9. How published ");
            attroff(A_BOLD);
          }
         else printw("%s","9. How published ");
         if (set_howpublished == 1) addch('*');
         print_pos++;
       }
      move(print_pos,5);
      print_pos+= field_menu(use_institution, set_institution, "A. Institution");
      move(print_pos,5);
      print_pos+= field_menu(use_journal, set_journal, "B. Journal");
      move(print_pos,5);
      print_pos+= field_menu(use_key, set_key, "C. Key");
      move(print_pos,5);
      print_pos+= field_menu(use_month, set_month, "D. Month");
      move(print_pos,5);
      print_pos+= field_menu(use_note, set_note, "E. Note");
      move(print_pos,5);
      print_pos+= field_menu(use_number, set_number, "F. Number");
      move(print_pos,5);
      print_pos+= field_menu(use_organisation, set_organisation, "G. Organisation");
      move(print_pos,5);
      print_pos+= field_menu(use_pages, set_pages, "H. Pages");
      move(print_pos, 5);
      print_pos+= field_menu(use_publisher, set_publisher, "I. Publisher");
      move(print_pos, 5);
      print_pos+= field_menu(use_school, set_school, "J. Academic institution");
      move(print_pos, 5);
      print_pos+= field_menu(use_series, set_series, "K. Series");
      move(print_pos, 5);
      print_pos+= field_menu(use_title, set_title, "L. Title");
      move(print_pos, 5);
      print_pos+= field_menu(use_type, set_type, "M. Type");
      move(print_pos, 5);
      print_pos+= field_menu(use_volume, set_volume, "N. Volume");
      move(print_pos, 5);
      print_pos+= field_menu(use_year, set_year, "O. Year");

      refresh();
      cbreak();
      ch = getch();
      lch = tolower(ch);
    
      if (lch == 'q') return(0);

      setup_screen();
      move(4,5);
      nocbreak();

      switch(lch)
       {
         case '1' : printw("%s","Please enter the address of the publisher or other institution");
            move(5,5);
            printw("%s","For major publishing houses it is sufficient to give just the city");
            move(7,5); refresh();
            getstr(address);
            set_address = 1; break;
         case '2' : printw("%s","Please enter an annotation");
            move(6,5); refresh();
            getstr(annote); set_annote = 1; break;
         case '3' : printw("%s","Please enter the name(s) of the author(s).");
            move(6,5);
            printw("%s","These may be in the form:"); move(7,8);
            printw("%s","John Joseph Bloggs"); move(8,5);printw("%s","or");
            move(9,8); printw("%s","Bloggs, John Joseph"); move(11,5);
            printw("%s","Names should be separated by the word 'and', like so:");
            move(12,8); printw("%s","John Joseph Bloggs and Richard Lee Head");
            move(14,5); printw("%s","Double surnames without separating hyphens must be enclosed within braces");
            move(16,5);
            printw("%s","Words added to surnames such as von or de may be entered in either form:");
            move(17,8);
            printw("%s","Richard von Mannheim    or   von Mannheim, Richard");
            move(19,5); refresh(); getstr(author); set_author=1; break; 
         case '4' : printw("%s","Enter title of book which contains reference"); 
            move(6,5); refresh(); getstr(booktitle); set_booktitle=1; break;
         case '5' : printw("%s","Please enter chapter or section number");
            move(6,5); refresh(); getstr(chapter); set_chapter=1; break;
         case '6' : printw("%s","Please enter the entry to cross-reference to.");
            move(6,5); refresh(); getstr(crossref);set_crossref=1;break;
         case '7' : printw("%s","Please enter the edition.");
            move(5,5);printw("%s","This should be written in full and be capitalised. For example:");
            move(6,8); printw("%s","Second"); move(8,5); refresh();
            getstr(edition); set_edition=1; break;
         case '8' : printw("%s","Please enter the name(s) of the editor(s).");
            move(5,5); printw("%s","These should be in the same format as the author name(s)");
            move(7,5); refresh(); getstr(editor);
            set_editor = 1; break;
         case '9' : printw("%s","Please enter anything unusual about the method of publishing.");
            printw("%s","This should be capitalised. For example:");
            move(5,8); printw("%s","Privately Published");
            move(7,5); refresh(); getstr(howpublished); set_howpublished=1;
            break;
         case 'a' : printw("%s","Please enter the name of the institution");
            move(6,5); refresh(); getstr(institution); set_institution=1;
            break;
         case 'b' : printw("%s","Please enter the name of the journal or magazine");
            move(6,5); refresh(); getstr(journal); set_journal=1; break;
         case 'c' : printw("%s","Please enter a key.");
            printw("%s","This is for additional alphabetising when the author");
            move(5,5);
            printw("%s","information is missing and is not the same as the entry key.");
            move(7,5); refresh(); getstr(key); set_key=1; break;
         case 'd' : printw("%s","Please enter the month in which the work was published");
            move(6,5);
            printw("%s","If it is unpublished, enter the month it was written");
            move(8,5); refresh(); getstr(month); set_month=1; break;
         case 'e' : printw("%s","Please enter any additional information");
            move(5,5); printw("%s","The first letter should be uppercase");
            move(7,5); refresh(); getstr(note); set_note = 1; break;
         case 'f' : printw("%s","Please enter the number of the journal, book, etc");
            move(6,5); refresh(); getstr(number); set_number=1; break;
         case 'g' : printw("%s","Please enter the sponsoring organisation");
            move(6,5); refresh(); getstr(organisation); set_organisation=1; break;
         case 'h' : printw("%s","Please enter a page number or range of pages");
            move(5,5);
            printw("%s","This can take the following forms:");
            move(6,8); printw("%s","33,44,56");
            move(7,8); printw("%s","45--117");
            move(8,8); printw("%s","87+");
            move(10,5);refresh();getstr(pages);set_pages=1;break;
         case 'i' : printw("%s","Please enter the name of the publisher");
            move(6,5);refresh();getstr(publisher);set_publisher=1; break;
         case 'j' : printw("%s","Please enter the name of the academic institution");
            move(6,5);refresh();getstr(school);set_school=1;break;
         case 'k' : printw("%s","Please enter the name of the series or set of books");
            move(5,5);
            printw("%s","When citing a book from a series, the title field gives the name");
            move(6,5);
            printw("%s","of the book itself while series gives the title of the whole set");
            move(8,5);refresh();getstr(series);set_series=1;break;
         case 'l' : printw("%s","Please enter the title of the work");
            move(6,5);
            refresh();
            getstr(title);
            set_title=1;
            break;
         case 'm' : printw("%s","Please enter the type, such as 'Research Note'");
            move(6,5); refresh(); getstr(type);set_type=1;break;
         case 'n' : printw("%s","Please enter the volume number"); move(6,5);
            refresh(); getstr(volume); set_volume=1; break;
         case 'o' : printw("%s","Please enter the year of publication.");
            move(5,5);
            printw("%s","If the work is unpublished, enter the year it was written");
            move(6,5);
            printw("%s","Years should be given in the four-digit form, ie. 1999");
            move(8,5);refresh();getstr(year);set_year=1;break;
         default : break;
       }

      /* Check to see if we have all the info we need */
      if (ch == 'z')
       {
         not_done =0;
         if (use_address==1 && set_address == 0)
            not_done++;
         if (use_annote==1 && set_annote==0)
            not_done++;
         if (use_author==1 && set_author==0)
            not_done++;
         if (use_booktitle==1 && set_booktitle==0)
            not_done++;
         if (use_chapter==1 && set_chapter==0)
            not_done++;
         if (use_edition==1 && set_edition==0)
            not_done++;
         if (use_editor==1 && set_editor==0)
            not_done++;
         if (use_howpublished==1 && set_howpublished==0)
            not_done++;
         if (use_institution==1 && set_institution==0)
            not_done++;
         if (use_journal==1 && set_journal==0)
            not_done++;
         if (use_key==1 && set_key ==0)
            not_done++;
         if (use_month==1 && set_month==0)
            not_done++;
         if (use_note==1 && set_note==0)
            not_done++;
         if (use_number==1 && set_number==0)
            not_done++;
         if (use_organisation==1 && set_organisation==0)
            not_done++;
         if (use_pages==1 && set_pages==0)
            not_done++;
         if (use_publisher==1 && set_publisher==0)
            not_done++;
         if (use_school==1 && set_school==0)
            not_done++;
         if (use_series==1 && set_series==0)
            not_done++;
         if (use_title==1 && set_title==0)
            not_done++;
         if (use_type==1 && set_type==0)
            not_done++;
         if (use_volume==1 && set_volume==0)
            not_done++;
         if (use_year==1 && set_year==0)
            not_done++;
         if (not_done > 0)
          {
            setup_screen();
            move(4,5);
            printw("%s %d %s","You can't finish yet - there are still",not_done,"required fields missing!");
            move(10,5);
            printw("%s","Press a key");
            cbreak();
            ch = getch();
          }
         else
            still_setting_up = 1;
       }
    }

   /* Create a key name for the new entry */
   setup_screen();
   move(4,5);
   printw("%s","Please enter an entry name for the new reference ");
   move(5,5);
   refresh();
   nocbreak();
   getstr(entry_name);

   /* Add our new entry to the output file */
   fin = fopen(bibfile,"r");
   if (fin == NULL)
    {
      endwin();
      perror("Can't open BibTeX file for reading ");
      exit(1);
    }

   fout = fopen("bibcursed_tmp.out","w");
   if (fout == NULL)
    {
      endwin();
      perror("Can't write output file ");
      exit(1);
    }

   while((fgets(str, 99, fin)) > 0)
    {
      if (strchr(str, '@') != NULL)
       {
    	 entry_count++;

         /* We've found a reference name */
         strcpy(str2, str);
         if (strchr(str2,'{') != NULL)
             key_name = (strchr(str2,'{')+1);
         else
             key_name = (strchr(str2,'(')+1);
          
         length = strlen(key_name);
         *(key_name + (length-2)) = 0;

         /* Check we haven't already used the entry name */
         if (strcmp(key_name,entry_name) == 0)
           written_new = 2; 

         /* If the current name comes after our new one, write the new one */
         if (strcmp(key_name, entry_name) > 0)
          {
            if (written_new == 0)
             {
               /* Write entire new entry to file */
               switch (entry_type)
                {
                  case '1' :
                     fprintf(fout, "@article{%s", entry_name); break;
                  case '2' :
                     fprintf(fout,"@book{%s", entry_name); break;
                  case '3' :
                     fprintf(fout,"@booklet{%s", entry_name); break;
                  case '4' :
                     fprintf(fout,"@inproceedings{%s", entry_name);
                     break;
                  case '5' :
                     fprintf(fout,"@inbook{%s", entry_name); break;
                  case '6' :
                     fprintf(fout,"@incollection{%s", entry_name); break;
                  case '7' :
                     fprintf(fout,"@manual{%s", entry_name); break;
                  case '8' :
                     fprintf(fout,"@mastersthesis{%s", entry_name); break;
                  case '9' :
                     fprintf(fout,"@misc{%s", entry_name); break;
                  case 'a' :
                     fprintf(fout,"@phdthesis{%s", entry_name); break;
                  case 'b' :
                     fprintf(fout,"@proceedings{%s", entry_name); break;
                  case 'c' :
                     fprintf(fout,"@techreport{%s", entry_name); break;
                  case 'd' :
                     fprintf(fout,"@unpublished{%s", entry_name); break;
                  default : break;
                }
               if (set_key == 1)
                  fprintf(fout,",\n  KEY={%s}", key);
               if (set_crossref == 1)
                  fprintf(fout,",\n  CROSSREF={%s}", crossref);
               if (set_author == 1)
                  fprintf(fout,",\n  AUTHOR={%s}",author);
               if (set_title == 1)
                  fprintf(fout,",\n  TITLE={%s}",title);
               if (set_booktitle == 1)
                  fprintf(fout,",\n  BOOKTITLE={%s}", booktitle);
               if (set_journal == 1)
                  fprintf(fout,",\n  JOURNAL={%s}", journal);
               if (set_series == 1)
                  fprintf(fout,",\n  SERIES={%s}", series);
               if (set_edition == 1)
                  fprintf(fout,",\n  EDITION={%s}", edition);
               if (set_volume == 1)
                  fprintf(fout,",\n  VOLUME={%s}", volume);
               if (set_number == 1)
                  fprintf(fout,",\n  NUMBER={%s}", number);
               if (set_type == 1)
                  fprintf(fout,",\n  TYPE={%s}", type);
               if (set_chapter == 1)
                  fprintf(fout,",\n  CHAPTER={%s}", chapter);
               if (set_pages == 1)
                  fprintf(fout,",\n  PAGES={%s}", pages);
               if (set_editor == 1)
                  fprintf(fout,",\n  EDITOR={%s}", editor);
               if (set_institution == 1)
                  fprintf(fout,",\n  INSTITUTION={%s}", institution);
               if (set_organisation == 1)
                  fprintf(fout,",\n  ORGANIZATION={%s}", organisation);
               if (set_school == 1)
                  fprintf(fout,",\n  SCHOOL={%s}", school);
               if (set_publisher == 1)
                  fprintf(fout,",\n  PUBLISHER={%s}", publisher);
               if (set_address == 1)
                  fprintf(fout,",\n  ADDRESS={%s}",address);
               if (set_howpublished == 1)
                  fprintf(fout,",\n  HOWPUBLISHED={%s}", howpublished);
               if (set_month == 1)
                  fprintf(fout,",\n  MONTH={%s}", month);
               if (set_year == 1)
                  fprintf(fout,",\n  YEAR={%s}", year);
               if (set_annote == 1)
                  fprintf(fout,",\n  ANNOTE={%s}",annote);
               if (set_note == 1)
                  fprintf(fout,",\n  NOTE={%s}", note);
               fprintf(fout,"\n }\n\n");
               written_new = 1;
               rec_count++;
             }
          }
       }
      /* Write the input line to the output file */
      fputs(str, fout);
    }
   
   //if (entry_count == 0)
   if (written_new == 0)
    {
      /* This is a bit of a hack. This function adds the new record when it
	 comes across a record that comes after it. If the new record comes
	 after all the present ones, or the bibfile is empty, it won't
	 work. This bit of code writes the new record at the end of the
	 file if no other records were found */

      switch (entry_type)
       {
         case '1' :
            fprintf(fout, "@article{%s", entry_name); break;
         case '2' :
            fprintf(fout,"@book{%s", entry_name); break;
         case '3' :
            fprintf(fout,"@booklet{%s", entry_name); break;
         case '4' :
            fprintf(fout,"@inproceedings{%s", entry_name);
            break;
         case '5' :
            fprintf(fout,"@inbook{%s", entry_name); break;
         case '6' :
            fprintf(fout,"@incollection{%s", entry_name); break;
         case '7' :
            fprintf(fout,"@manual{%s", entry_name); break;
         case '8' :
            fprintf(fout,"@mastersthesis{%s", entry_name); break;
         case '9' :
            fprintf(fout,"@misc{%s", entry_name); break;
         case 'a' :
            fprintf(fout,"@phdthesis{%s", entry_name); break;
         case 'b' :
            fprintf(fout,"@proceedings{%s", entry_name); break;
         case 'c' :
            fprintf(fout,"@techreport{%s", entry_name); break;
         case 'd' :
            fprintf(fout,"@unpublished{%s", entry_name); break;
         default : break;
       }
      if (set_key == 1)
         fprintf(fout,",\n  KEY={%s}", key);
      if (set_crossref == 1)
         fprintf(fout,",\n  CROSSREF={%s}", crossref);
      if (set_author == 1)
         fprintf(fout,",\n  AUTHOR={%s}",author);
      if (set_title == 1)
         fprintf(fout,",\n  TITLE={%s}",title);
      if (set_booktitle == 1)
         fprintf(fout,",\n  BOOKTITLE={%s}", booktitle);
      if (set_journal == 1)
         fprintf(fout,",\n  JOURNAL={%s}", journal);
      if (set_series == 1)
         fprintf(fout,",\n  SERIES={%s}", series);
      if (set_edition == 1)
         fprintf(fout,",\n  EDITION={%s}", edition);
      if (set_volume == 1)
         fprintf(fout,",\n  VOLUME={%s}", volume);
      if (set_number == 1)
         fprintf(fout,",\n  NUMBER={%s}", number);
      if (set_type == 1)
         fprintf(fout,",\n  TYPE={%s}", type);
      if (set_chapter == 1)
         fprintf(fout,",\n  CHAPTER={%s}", chapter);
      if (set_pages == 1)
         fprintf(fout,",\n  PAGES={%s}", pages);
      if (set_editor == 1)
         fprintf(fout,",\n  EDITOR={%s}", editor);
      if (set_institution == 1)
         fprintf(fout,",\n  INSTITUTION={%s}", institution);
      if (set_organisation == 1)
         fprintf(fout,",\n  ORGANIZATION={%s}", organisation);
      if (set_school == 1)
         fprintf(fout,",\n  SCHOOL={%s}", school);
      if (set_publisher == 1)
         fprintf(fout,",\n  PUBLISHER={%s}", publisher);
      if (set_address == 1)
         fprintf(fout,",\n  ADDRESS={%s}",address);
      if (set_howpublished == 1)
         fprintf(fout,",\n  HOWPUBLISHED={%s}", howpublished);
      if (set_month == 1)
         fprintf(fout,",\n  MONTH={%s}", month);
      if (set_year == 1)
         fprintf(fout,",\n  YEAR={%s}", year);
      if (set_annote == 1)
         fprintf(fout,",\n  ANNOTE={%s}",annote);
      if (set_note == 1)
         fprintf(fout,",\n  NOTE={%s}", note);
      fprintf(fout,"\n }\n\n");
      written_new = 3;
      rec_count++;
    }
   fclose(fin);
   fclose(fout);

   /* Rename the original bibTeX file to bibfile.old */
   strcpy(out_string, "cp ");
   strcat(out_string, bibfile);
   strcat(out_string, " bibfile.old");
   system(out_string);

   /* Rename the output from bibcursed_tmp.out to the original filename */
   strcpy(out_string, "mv bibcursed_tmp.out ");
   strcat(out_string, bibfile);
   system(out_string);

   /* Report on success or otherwise */
   move(7,5);
   switch(written_new)
    {
      case 0 : printw("%s","No new entry was written. Press any key"); break;
      case 1 : printw("%s","New entry was successfully written. Press a key"); break;
      case 2 : printw("%s","Entry name already exists - new entry not added. Press a key");
               break;
      case 3 : printw("%s","New entry was added to the end of the file. Press a key"); break;
      default : break;
    }
   cbreak();
   ch = getch();

   return 0;
 }


int delete_entry()
 {
   char entry_name[100], str[100], str2[100], out_string[100], ch;
   char *key_name;
   FILE *fin, *fout;
   int record_found = 0, length, dont_output = 0, removed_something=0;

   setup_screen();
   move(4,5);
   printw("%s","Enter the name of the entry to delete");
   move(5,5);
   refresh();
   nocbreak();
   getstr(entry_name);

   fin = fopen(bibfile,"r");

   fout = fopen("bibcursed_tmp.out","w");
   if (fout == NULL)
    {
      endwin();
      perror("Could not open file for output ");
      exit(1);
    }

   while((fgets(str, 99, fin)) > 0)
    {
      if (strchr(str, '@') != NULL)
       {
         strcpy(str2, str);
         if (strchr(str2,'{') != NULL)
            key_name = (strchr(str2,'{')+1);
         else
            key_name = (strchr(str2,'(')+1);
         length = strlen(key_name);
         *(key_name + (length-2)) = 0;
         if (strcmp(key_name,entry_name) == 0)
          {
            /* Skip this entry */
            dont_output = 1;
            removed_something = 1;
            rec_count--;
            move(7,5);
            printw("%s removed",key_name);
          }
         else
          {
            dont_output = 0;
            fputs(str, fout);
          }
       }
      else
         if (dont_output == 0)
          {
            fputs(str, fout);
          }
    }
   fclose(fin);
   fclose(fout);

   move(10,5);
   if (removed_something == 0)
    {
      printw("%s","Record not found - nothing removed");
      move(12,5);
    }
   printw("%s","Press any key");
   cbreak();
   refresh();

   ch = getch();

   if (removed_something == 1)
    {
      /* Only do all the renaming and moving if we deleted something */
      strcpy(out_string, "cp ");
      strcat(out_string, bibfile);
      strcat(out_string, " bibfile.old");
      system(out_string);

      strcpy(out_string, "mv bibcursed_tmp.out ");
      strcat(out_string, bibfile);
      system(out_string);
    }

   return 0;
 }


int field_menu(int priority, int field_set, char *fieldname)
 {
   int ret_val = 0;
   if (priority != 0)
    {
      if (priority == 1)
       {
         attron(A_BOLD);
         printw("%s ", fieldname);
         attroff(A_BOLD);
       }
      else printw("%s ", fieldname);
      if (field_set == 1) addch('*');
      ret_val = 1;
    }
   return(ret_val);
 }


int change_entry()
 {
   char entry_name[100], str[100], ch, lch;
   int record_found = 0, found_record = 0, print_pos = 6, record_count = 0;
   FILE *fin;

   setup_screen();
   move(4,5);
   printw("%s","Please enter the record name : ");
   refresh();
   nocbreak();
   getstr(entry_name);

   fin = fopen(bibfile, "r");
   if (fin == NULL) return(1);

   move(print_pos,1);
   while ((fgets(str, 99, fin)) > 0)
    {
      if (strchr(str, '@') != NULL)
       {
         if (strstr(str,entry_name) != 0)
          {
            if (record_count > 0)
             {
               printw("%s","More than one match! Be more specific! Any key for next match");
               refresh();
               cbreak();
               ch = getch();
               setup_screen();
               move(4,0);
               refresh();
             }
            record_found = 1;
            found_record = 1;
            record_count++;
          }
         else
            record_found = 0;
       }
      if (record_found == 1)
       {
         printw("%s",str);
         print_pos++;
       }
    }
   fclose(fin);

   move(print_pos + 1, 5);
   if (found_record == 0)
    { 
      printw("%s","The record name was not found! ");
      printw("%s","Press a key");
      refresh();
      cbreak();
      ch = getch();
      return(0);
    }

   if (record_count > 1)
      printw("%s","You must give a unique ID to change a field - try again");
   else
      printw("%s","Press C to change a field or any other key to return to the menu");
   refresh();
   cbreak();
   ch = getch();
   lch = tolower(ch);

   if (lch == 'c')
      change_field(entry_name);

   return 0;
 }


int change_field(char *entry_name)
 {
   char ch, lch;
   char *field_name, *lfield_name, *key_name;
   char field_data[200], str[200], out_string[100];
   FILE *fin, *fout;
   int in_record = 0, changed_field = 0;

   setup_screen();
   move(4,5);
   printw("%s","Select a field to add/replace.");
   move(5,5); printw("%s","Note that not all are valid for every entry type!");
   move(6,5); printw("%s","If in doubt, don't add anything that isn't there already");
   move(8,8);  printw("%s","1. Address          D. Month");
   move(9,8);  printw("%s","2. Annote           E. Note");
   move(10,8); printw("%s","3. Author           F. Number");
   move(11,8); printw("%s","4. Booktitle        G. Organisation");
   move(12,8); printw("%s","5. Chapter          H. Pages");
   move(13,8); printw("%s","6. Crossref         I. Publisher");
   move(14,8); printw("%s","7. Edition          J. School");
   move(15,8); printw("%s","8. Editor           K. Series");
   move(16,8); printw("%s","9. Howpublished     L. Title");
   move(17,8); printw("%s","A. Institution      M. Type");
   move(18,8); printw("%s","B. Journal          N. Volume");
   move(19,8); printw("%s","C. Key              O. Year");

   move(21,8);printw("%s","Make your choice or press q to quit");

   refresh();
   cbreak();
   ch = getch();
   lch = toupper(ch);

   if (lch == 'q')
      return(0);

   switch(lch)
    {
      case '1' : field_name = "ADDRESS"; lfield_name="address"; break;
      case '2' : field_name = "ANNOTE"; lfield_name="annote"; break;
      case '3' : field_name = "AUTHOR"; lfield_name="author"; break;
      case '4' : field_name = "BOOKTITLE"; lfield_name="booktitle";break;
      case '5' : field_name = "CHAPTER"; lfield_name="chapter";break;
      case '6' : field_name = "CROSSREF"; lfield_name="crossref";break;
      case '7' : field_name = "EDITION"; lfield_name="edition";break;
      case '8' : field_name = "EDITOR"; lfield_name="editor";break;
      case '9' : field_name = "HOWPUBLISHED";lfield_name="howpublished";break;
      case 'A' : field_name = "INSTITUTION";lfield_name="institution";break;
      case 'B' : field_name = "JOURNAL";lfield_name="journal";break;
      case 'C' : field_name = "KEY";lfield_name="key";break;
      case 'D' : field_name = "MONTH";lfield_name="month";break;
      case 'E' : field_name = "NOTE";lfield_name="note";break;
      case 'F' : field_name = "NUMBER";lfield_name="number";break;
      case 'G' : field_name = "ORGANIZATION";lfield_name="organization";break;
      case 'H' : field_name = "PAGES"; lfield_name = "pages";break;
      case 'I' : field_name = "PUBLISHER";lfield_name = "publisher";break;
      case 'J' : field_name = "SCHOOL";lfield_name="school";break;
      case 'K' : field_name = "SERIES";lfield_name="series";break;
      case 'L' : field_name = "TITLE"; lfield_name="title"; break;
      case 'M' : field_name = "TYPE"; lfield_name="type";break;
      case 'N' : field_name = "VOLUME"; lfield_name="volume";break;
      case 'O' : field_name = "YEAR";lfield_name = "year"; break;
      default: return(0); break;
    }

   setup_screen();
   move(4,5);
   printw("%s %s %s","Enter data for",lfield_name,"and press return to finish");
   move(5,5);
   printw("%s","To delete the field, just press return");
   move(7,1);
   refresh();
   nocbreak();
   getstr(field_data);

   fin = fopen(bibfile,"r");
   if (fin == NULL) return(0);

   fout = fopen("bibcursed_tmp.out","w");
   if (fout == NULL)
    {
      endwin();
      perror("Couldn't open temp output file ");
      exit(1);
    }

   while((fgets(str, 99, fin)) > 0)
    {
      if (strchr(str, '@') != NULL)
       {
         if (strchr(str,'{') != NULL)
            key_name = (strchr(str,'{')+1);
         else
            key_name = (strchr(str,'(')+1);

         /* This is a record */
         if (strstr(key_name, entry_name) != NULL)
          {
            /* This is the right record */
            fputs(str,fout);
            if (strlen(field_data) > 0)
               fprintf(fout,"  %s={%s},\n",field_name,field_data);
            in_record = 1;
            changed_field = 1;
          }
         else
          {
            fputs(str,fout);
            in_record = 0;
          }
       }
      else
       {
         /* Make sure we don't write the new field we've just replaced */
         if (in_record == 1)
          {
            if (strstr(str,field_name) == NULL)
             {
               if (strstr(str,lfield_name) == NULL)
                  fputs(str,fout);
             }
          }
         else
            fputs(str,fout);
       }
    }

   fclose(fin);
   fclose(fout);

   strcpy(out_string, "cp ");
   strcat(out_string, bibfile);
   strcat(out_string, " bibfile.old");
   system(out_string);
   strcpy(out_string, "mv bibcursed_tmp.out ");
   strcat(out_string, bibfile);
   system(out_string);

   move(10,5);
   if (changed_field == 1)
      printw("%s","Change made - press a key");
   else
      printw("%s","No changes made - press a key");
  
   refresh();
   cbreak();
   ch = getch();
 
   return 0;
 }


int search_file(int what)
 {
   /* Search the file using the supplied option */
   char search_term[100], search_term_lower[100];
   char *key_name;
   char str[100], str2[100], str2_lower[100];
   char ch;
   char *search_type, *search_type2;
   FILE *fin;
   int print_pos = 6;
   int entries_found = 0;
   int author_found;
   int total_entries = 0;
   int every = 0;
   int i;

   setup_screen();
   move(4,5);
   printw("%s", "Please enter ");
   switch(what)
    {
      case 1 : printw("%s","author surname "); search_type = "AUTHOR"; 
               search_type2 = "author"; break;
      case 2 : printw("%s","principle author surname "); search_type =
               "AUTHOR"; search_type2 = "author"; break;
      case 3 : printw("%s","surname of author to link to ");
               search_type="AUTHOR"; search_type2 = "author"; break;
      case 4 : printw("%s","title (or partial title) "); search_type = "TITLE";
               search_type2 = "title"; break;
      case 5 : printw("%s","conference or journal name "); search_type = 
               "JOURNAL"; search_type2 = "journal"; break;
      case 6 : printw("%s","name of university or institution "); 
               search_type = "SCHOOL"; search_type2 = "school"; break;
      case 7 : printw("%s","year "); search_type = "YEAR"; 
               search_type2 = "year"; break;
      case 8 : printw("%s","keyword "); search_type2 = "all"; every = 1; break;
      case 9 : printw("%s","name of organisation ");
               search_type = "ORGANIZATION"; search_type2="organization"; break;
      default : printw("%s","nothing (error)\n"); search_menu(); break;
    }
   printf("for search : ");
   refresh();

   getstr(search_term);
   
   /* Convert search term to lowercase */
   for(i = 0; search_term[i]; i++) {
     search_term_lower[i] = tolower(search_term[i]);
   }
   search_term_lower[i] = '\0';

   setup_screen();
   move(4,5);
   printw("%s","Searching for ");
   attron(A_BOLD);
   printw("%s", search_term);
   attroff(A_BOLD);
   refresh();

   fin = fopen(bibfile, "r");
   if (fin == NULL)
    {
      perror("Cant open BibTeX file ");
      exit(1);
    }

   while ((fgets(str, 99, fin)) > 0)
    {
      author_found = 0;
      if (strchr(str, '@') != NULL)
       {
         if (strchr(str, '{') != NULL)
          {
            key_name = (strchr(str, '{') + 1);
          }
         else
          {
            key_name = (strchr(str, '(') + 1);
          }
         while (author_found == 0)
          {
            if (fgets(str2, 99, fin) == NULL) break;
            if ((strstr(str2, search_type) != NULL) || (strstr(str2, search_type2)
               != NULL) || every == 1)
             {
               /* Convert str2 to lowercase for comparison */
               strcpy(str2_lower, str2);
               for(i = 0; str2_lower[i]; i++) {
                 str2_lower[i] = tolower(str2_lower[i]);
               }
               
               if (strstr(str2_lower, search_term_lower) != NULL)
                {
                  move(print_pos,5);
                  printw("%s", key_name);
                  move(print_pos+1, 8);
                  printw("%s", str2);
                  print_pos = print_pos + 2;
                  entries_found++;
                  total_entries++;
                  if (entries_found == ((LINES - 9)/2))
                   {
                     move(print_pos+1,5);
                     printw("%s","Press any key for more results");
                     refresh();
                     cbreak();
                     ch = getch();
                     entries_found = 0;
                     print_pos = 6;
                     clear();
                     setup_screen();
                     move(4,5);
                     printw("%s","Searching for ");
                     attron(A_BOLD);
                     printw("%s", search_term);
                     attroff(A_BOLD);
                   }
                  refresh();
                }
               author_found = 1;
             }
          }
       }
    }

   fclose(fin);
   move(print_pos+1,5);
   printw("%s %d %s","That's it -", total_entries, "match. Press any key");
   refresh();
   cbreak();
   ch = getch();

   return 0;
 }


void add_bold(char letter)
 {
   /* Add the supplied char in bold to the current position */
   attron(A_BOLD);
   addch(letter);
   attroff(A_BOLD);
 }


int search_menu()
 {
   /* Display the first search menu */
   int ch, lch, type;

   setup_screen();
   move(4,5);
   printw("Search by :");
   move(6,8);
   add_bold('A');
   printw("uthor");
   move(7,8);
   add_bold('P');
   printw("%s","rinciple author only");
   move(8,8);
   add_bold('L');
   printw("%s","inked authors");
   move(9,8);
   add_bold('T');
   printw("%s","itle");
   move(10,8);
   add_bold('J');
   printw("%s","ournal or conference title");
   move(11,8);
   add_bold('U');
   printw("%s","niversity or institution (for thesis)");
   move(12,8);
   add_bold('O');
   printw("%s","rganisation (for proceedings etc)");
   move(13,8);
   add_bold('Y');
   printw("%s","ear");
   move(14,8);
   add_bold('E');
   printw("%s","verything (keyword for all fields)");
   move(15,8);
   add_bold('Q');
   printw("%s","uit and return to main menu");

   move(17,8);
   printw("What now? ");
   refresh();

   cbreak();
   ch = getch();
   lch = tolower(ch);

   switch(lch)
    {
      case 'a' : return search_file(1);
      case 'p' : return search_file(2);
      case 'l' : return search_file(3);
      case 't' : return search_file(4);
      case 'j' : return search_file(5);
      case 'u' : return search_file(6);
      case 'y' : return search_file(7);
      case 'e' : return search_file(8);
      case 'o' : return search_file(9);
      case 'q' : return 0;
      default : menu(); return 0;
    }
 }


int quit_prog()
 {
   endwin();
   exit(0);
   return 0;  // This line will never be reached
 }


int list_all_entries()
 {
   char str[100], str2[100], ch, lch;
   FILE *fin;
   int print_pos = 7;
   int entries_displayed = 0;
   int current_pos = 0;
   int total_entries = 0;
   int max_entries_per_page;
   int is_last_record = 0;
   char *key_name;
   char *field, *value;
   char *equals_pos;
   int done = 0;

   setup_screen();
   move(4,5);
   printw("%s","Loading entries...");
   refresh();

   fin = fopen(bibfile, "r");
   if (fin == NULL)
    {
      move(6,5);
      printw("%s %s","Error: Could not find file", bibfile);
      refresh();
      cbreak();
      ch = getch();
      return 0;
    }

   /* First count total entries */
   while ((fgets(str, 99, fin)) > 0)
    {
      if (strchr(str, '@') != NULL)
       {
         total_entries++;
       }
    }
   rewind(fin);

   if (total_entries == 0)
    {
      move(6,5);
      printw("%s","No entries found in the file");
      refresh();
      cbreak();
      ch = getch();
      fclose(fin);
      return 0;
    }

   /* Calculate how many entries can fit on a page */
   max_entries_per_page = (LINES - 8) / 3;  /* Conservative estimate */

   while (!done)
    {
      setup_screen();
      print_pos = 7;
      entries_displayed = 0;

      /* Display entries */
      while ((fgets(str, 99, fin)) > 0)
       {
         if (strchr(str, '@') != NULL)
          {
            if (entries_displayed >= max_entries_per_page)
             {
               /* We've filled the page */
               break;
             }

            /* Get entry key */
            if (strchr(str, '{') != NULL)
             {
               key_name = (strchr(str, '{') + 1);
             }
            else
             {
               key_name = (strchr(str, '(') + 1);
             }
            str2[strlen(key_name)-2] = 0;  /* Remove closing brace/parenthesis */

            /* Display entry type line */
            move(print_pos, 5);
            attron(A_BOLD);
            printw("%s", str);
            attroff(A_BOLD);
            print_pos++;

            /* Display fields */
            while ((fgets(str, 99, fin)) > 0)
             {
               if (strchr(str, '@') != NULL)
                {
                  /* We've hit the next entry */
                  ungetc('@', fin);  /* Put back the @ for next iteration */
                  break;
                }

               if (print_pos >= LINES - 1)
                {
                  /* We've hit the bottom of the screen */
                  break;
                }

               /* Format and display field */
               equals_pos = strchr(str, '=');
               if (equals_pos != NULL)
                {
                  /* Split into field and value */
                  field = str;
                  *equals_pos = 0;
                  value = equals_pos + 1;

                  /* Trim whitespace */
                  while (*field == ' ') field++;
                  while (*value == ' ') value++;

                  /* Display field name in bold */
                  move(print_pos, 5);
                  attron(A_BOLD);
                  printw("%-20s", field);
                  attroff(A_BOLD);

                  /* Display value */
                  printw("%s", value);
                }
               else
                {
                  move(print_pos, 5);
                  printw("%s", str);
                }

               print_pos++;
             }

            entries_displayed++;
            print_pos++;  /* Add blank line between entries */
          }
       }

      /* Update status line */
      move(4, 5);
      printw("Showing entries %d to %d of %d", 
             current_pos + 1, 
             current_pos + entries_displayed, 
             total_entries);

      /* Check if we're at the last record */
      is_last_record = (current_pos + entries_displayed >= total_entries);
      move(5, 5);
      if (is_last_record)
       {
         printw("Last record - Press P for previous page, or Q to quit");
       }
      else
       {
         printw("Press N for next page, P for previous page, or Q to quit");
       }

      refresh();

      /* Handle navigation */
      while (1)
       {
         cbreak();
         ch = getch();
         lch = tolower(ch);

         if (lch == 'n' && !is_last_record)
          {
            current_pos += entries_displayed;
            break;
          }
         else if (lch == 'p' && current_pos > 0)
          {
            current_pos = (current_pos - max_entries_per_page > 0) ? 
                         current_pos - max_entries_per_page : 0;
            break;
          }
         else if (lch == 'q')
          {
            fclose(fin);
            return 0;
          }
       }

      /* Reset file position for next page */
      rewind(fin);
      /* Skip entries we've already shown */
      for (int i = 0; i < current_pos; i++)
       {
         int found_entry = 0;
         while ((fgets(str, 99, fin)) > 0)
          {
            if (strchr(str, '@') != NULL)
             {
               found_entry = 1;
               break;
             }
          }
         if (!found_entry || feof(fin)) {
           fclose(fin);
           return 0;  /* Return if we can't find enough entries or hit EOF */
         }
       }
    }
   fclose(fin);
   return 0;
 }


int main(int argc, char *argv[])
 {
   int ch, i;
   char str[100];
   FILE *fin;


   printf("Bibcursed 2.0.1 (c)1999-2025 Dominic Tristram. GPL applies\n");
   sleep(1);

   if (argc != 2)
    {
      printf("Please enter a file name: ");
      fgets(bibfile,100,stdin);
      // Remove newline
      strtok(bibfile,"\n");
    }
   else
    {
      strcpy(bibfile, argv[1]);
    }
   if (access(bibfile, R_OK) < 0)
    {
     fin = fopen(bibfile, "a+");
    }
   else
    {
      fin = fopen(bibfile, "r");
    }
   if (fin == NULL)
    {
      printf("Can't open %s for reading\n", bibfile);
      exit(1);
    }

   while ((fgets(str, 10, fin)) > 0)
    {
      if (str[0] == '@') rec_count++;
    }

   printf("There are %d BibTeX entries in %s\n", rec_count, argv[1]);
   fclose(fin);

   initscr();

   while (1 != 0)
      menu();

   return 0;
 }
