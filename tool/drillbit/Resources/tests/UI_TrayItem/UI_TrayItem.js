/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

describe("UI.Tray Tests", {
  test_ui_module_methods: function () {
    value_of(Ti.UI.addTray)
      .should_be_function();
    value_of(Ti.UI.clearTray)
      .should_be_function();
    value_of(Ti.UI.Tray)
      .should_not_be_null();
  },

  test_tray_object: function () {
    var trayObj = Ti.UI.addTray("app://logo_small.png", function () {});

    value_of(trayObj)
      .should_be_object();

    value_of(trayObj.getHint)
      .should_be_function();
    value_of(trayObj.getIcon)
      .should_be_function();
    value_of(trayObj.getMenu)
      .should_be_function();
    value_of(trayObj.remove)
      .should_be_function();
    value_of(trayObj.setHint)
      .should_be_function();
    value_of(trayObj.setIcon)
      .should_be_function();
    value_of(trayObj.setMenu)
      .should_be_function();

    Ti.UI.clearTray();
  },

  test_tray_hint: function () {
    var trayObj = Ti.UI.addTray("app://logo_small.png", function () {});

    value_of(trayObj)
      .should_be_object();

    trayObj.setHint("get a clue");
    value_of(trayObj.getHint())
      .should_be("get a clue");

    Ti.UI.clearTray();
  },

  test_tray_icon: function () {
    var trayObj = Ti.UI.addTray("app://logo_small.png", function () {});

    value_of(trayObj)
      .should_be_object();
    value_of(trayObj.setIcon)
      .should_be_function();
    value_of(trayObj.getIcon)
      .should_be_function();

    value_of(trayObj.getIcon())
      .should_be("app://logo_small.png");

    trayObj.setIcon("app://tray.png");
    value_of(trayObj.getIcon())
      .should_be("app://tray.png");

    Ti.UI.clearTray();
  },

  test_tray_menu: function () {
    var trayObj = Ti.UI.addTray("app://logo_small.png", function () {});
    var menu = Ti.UI.createMenu();

    value_of(trayObj)
      .should_be_object();
    value_of(menu)
      .should_be_object();

    value_of(trayObj.getMenu)
      .should_be_function();
    value_of(trayObj.setMenu)
      .should_be_function();

    // no menu set, should be null
    value_of(trayObj.getMenu())
      .should_be_null();

    trayObj.setMenu(menu);
    value_of(trayObj.getMenu())
      .should_be_object();

    var theMenu = trayObj.getMenu();

    value_of(theMenu.equals(menu))
      .should_be_true();

    Ti.UI.clearTray();
  },

  test_tray_remove: function () {
    var trayObj = Ti.UI.addTray("app://logo_small.png", function () {});

    value_of(trayObj)
      .should_be_object();
    value_of(trayObj.remove)
      .should_be_function();

    // no real way to test this, but insure that the object won't crash if 
    // you call the methods.
    trayObj.remove();

    value_of(trayObj.getMenu())
      .should_be_null();
    value_of(trayObj.getIcon())
      .should_be("app://logo_small.png");
  }
});
