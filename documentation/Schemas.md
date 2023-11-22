```sql
CREATE TABLE Categories (
    category_id INT PRIMARY KEY AUTO_INCREMENT, 
    -- id handled by database
    category_name VARCHAR(63) UNIQUE NOT NULL,
    -- category needs (not null) a unique name
    category_description TEXT,                
    -- additional info about category
    category_visibility BOOLEAN NOT NULL DEFAULT 1
    -- indicates whether a category is active (displayed and accessible) or not
);
/* CategoriesList keeps a centralized list of all different keywords that can be associated 
with an item such that it can be searched as sets, intersections and unions */

CREATE TABLE ElementCategories (
	elemcat_timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,

    elemcat_item_guid VARCHAR(6) NOT NULL,        
    -- base_32 GUID of existing item to categorize
    FOREIGN KEY (elemcat_item_guid) REFERENCES Items(element_guid) ON DELETE CASCADE,
    -- ensures item_guid exists in the Item table
    -- when an item is deleted, its related category assignments are as well

    elemcat_category_id INT NOT NULL,            
    -- assignment of category from CategoriesList
    FOREIGN KEY (elemcat_category_id) REFERENCES Categories(category_id) ON DELETE CASCADE,
    -- ensures category_id exists in the Category table
    -- when a category is deleted, all items with such a category lose that category

    itemcategory_visibility BOOLEAN NOT NULL DEFAULT 1,
    -- indicates whether an association is active (displayed and accessible) or not
    
    PRIMARY KEY (elemcat_item, elemcat_category)        
    -- this relationship uniquely identifies an entry
);
/* ItemCategories keeps a EAV (entity-attribute-value) model with a foreign key constraint
Entity: item_guid, 
Attribute: category_id, 
Value: the relation between the two which is bound by a unique */

CREATE TABLE Users (
    user_username VARCHAR(31) PRIMARY KEY, -- Unchangeable username/ID 
    user_password_salthash VARCHAR(255) NOT NULL, -- Hash of the user's password 
    user_timestamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    -- DateTime of registration
    
    user_failed_logins INT NOT NULL DEFAULT 0, 
    -- exceeding a certain limit indicates brute force 
    -- account will lock until admin intervention reviewing logs and then resetting
    -- count also resets at start of program execution
    
    user_visibility BOOLEAN NOT NULL DEFAULT 1, 
    -- Activity status (active/inactive)
    -- indicates whether account is hidden or visible on lists
    
    user_permission ENUM('LOCK', 'BASE', 'SUPER', 'ADMIN') NOT NULL DEFAULT 'BASE',
    -- Permission level, each command knows what roles to allow to use it
    -- locked accounts are logged out and not allowed to log in again
    -- admin is required to reset the locked account, advising password change
    -- guest is the implicit state at the login screen
    -- commands like register, login, locked, info
    -- base has all view-only commands to observe the data but not manipulate
    -- admin role has all permissions including over base and super users
    -- super(visor) role has the permissions to edit data along with admin
    
    user_legal_name VARCHAR(63),  -- name user identifies with in real life
    user_phone_number VARCHAR(15), -- iso standard phone number length (omit non numerals)
    user_email_address VARCHAR(255), -- online email address (verify contains @ and . after)
    -- these fields are mutable

    user_note TEXT,
);
/* Relational table, note: append only, users cannot be deleted once created since records
need to be kept of who owns and has contributed what. Only an owner or Admin can transfer 
ownership of an item to another owner */

CREATE TABLE Element (
    element_guid VARCHAR(6) PRIMARY KEY,
    -- globally unique base_32 guid
    
    element_name VARCHAR(63) NOT NULL,
    -- lexical identifier for everyday use
    element_description TEXT,
	-- lexical description of an element, info such as address
	
	element_class ENUM('WAREHOUSE', 'STOREROOM', 'BAY', 'RACK', 'SHELF', 'CONTAINER', 'ITEM', 'BULK', 'MATERIAL') NOT NULL,
    
    element_parent_guid VARCHAR(6), -- parent guid, foreign key ensures existence
    FOREIGN KEY (element_parent) REFERENCES Items(element_guid) ON DELETE CASCADE,
    -- deleting an item will delete all of its children, so contingencies to
    -- relocate these children to the parent above need to be implemented 
    -- only an admin can call true deletion with password confirmation
        
    element_owner_username VARCHAR(31) NOT NULL,
    -- username of the user who created an item
    FOREIGN KEY (element_owner) REFERENCES Users(user_username)

    element_thumbnail_filepath TEXT,
    
    element_visibility BOOLEAN NOT NULL DEFAULT 1, 
    element_timestamp TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP 
);
/* Relational table of nodes in a non-self non-child nesting tree structure, with the root 
node being null and a few root tree nodes such as 000000 as root and 111111 as deprecated. 
For the deletion of an item, have the user retype the GUID as a sanity check and select a list of contingencies from assign to Parent, assign to root, and deprecate, true deletion only admin with password confirmation*/


```