<?php
/**
 * @file
 * webform_commerce.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
 
function webform_commerce_default_rules_configuration() {
  $rules = array();
  
  $rule = rules_reaction_rule();
  $rule->label = t("Update Webform Submission to Paid");
  $rule->active = TRUE;

  $rule
    ->event('commerce_checkout_complete')
    ->action('webform_commerce_update_status', array(
      'commerce_order' => 'commerce_order',
      ));

  $rules['update_webform_to_paid'] = $rule;
  
  $rule = entity_import('rules_config', '{
    "add_extra_line_items" : {
      "LABEL" : "Add extra line items to order",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "rules", "webform_commerce", "commerce_cart" ],
      "ON" : [ "commerce_cart_product_add" ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "commerce_line_item",
              "param_order" : [ "commerce-order" ],
              "param_type" : "webform_commerce",
              "param_line_item_label" : "Test Rule Line Item",
              "param_quantity" : "1",
              "param_commerce_unit_price" : { "value" : { "amount" : "100.00", "currency_code" : "USD" } },
              "param_commerce_total" : { "value" : { "amount" : "100.00", "currency_code" : "USD" } }
            },
            "PROVIDE" : { "entity_created" : { "entity_created" : "Created entity" } }
          }
        },
        { "entity_save" : { "data" : [ "entity-created" ], "immediate" : 1 } },
        { "webform_commerce_add_entity_to_cart" : {
            "commerce_line_item" : [ "entity-created" ],
            "commerce_order" : [ "entity-created:order" ]
          }
        }
      ]
    }
  }');

  $rules['add_extra_line_items'] = $rule;
  
  $rule = entity_import('rules_config', '{
    "add_components_to_line_item" : {
      "LABEL" : "Add components to line item",
      "PLUGIN" : "reaction rule",
      "REQUIRES" : [ "webform_commerce", "commerce_product_reference" ],
      "ON" : [ "commerce_product_calculate_sell_price" ],
      "IF" : [
        { "webform_contains_pricing_components" : { "commerce_line_item" : [ "commerce_line_item" ] } }
      ],
      "DO" : [
        { "webform_commerce_calculate_price" : {
            "commerce_line_item" : [ "commerce_line_item" ],
            "component_name" : "webform_fee"
          }
        }
      ]
    }
  }');
  
  $rules['add_components_to_line_item'] = $rule;
  
  return $rules;
}